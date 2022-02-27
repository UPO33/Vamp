#include "VampScene.h"
#include "SpriteBatch.h"
#include "VampEngine.h"
#include "imgui.h"
#include "GameplayConfig.h"


struct VSpriteVertexBuilder
{
	using VertexType = VSprite::Vertex;
	using IndexType = uint16_t;

	VArray<VertexType> mVertices;
	VArray<IndexType>  mIndices;
	int mIndexCount = 0;
	int mQuadCapacity = 0;


	void Reserve(size_t numQuad)
	{
		mQuadCapacity = numQuad;

		mVertices.SetCapacity(numQuad * 4);
		mIndices.SetCapacity(numQuad * 6);
	}
	void Reset()
	{
		mIndexCount = 0;
	}
	void CheckCap()
	{
		const int availQuad = mQuadCapacity - NumQuad();
		if (availQuad <= 0)
		{
			Reserve(mQuadCapacity + 2048);
		}
	}
	
	auto Vertices() { return mVertices.Elements(); }
	auto Indices() { return mIndices.Elements(); }

	int NumQuad() const { return mIndexCount / 6; }
	int NumVertex() const { return NumQuad() * 4; }
	int NumIndex() const { return mIndexCount; }

	static void InitQuadPositions(VertexType* baseVertex, const VVec2F& position, const VVec2F& pixelSize)
	{
		baseVertex[0].mPosition = position;
		baseVertex[1].mPosition = VVec2F(position.X + pixelSize.X, position.Y);
		baseVertex[2].mPosition = VVec2F(position.X, position.Y + pixelSize.Y);
		baseVertex[3].mPosition = position + pixelSize;
	}

	static void InitQuadUV(VertexType* baseVertex, const VSprite& sprite)
	{
		baseVertex[0].mUV = sprite.mQuadUV[0];
		baseVertex[1].mUV = sprite.mQuadUV[1];
		baseVertex[2].mUV = sprite.mQuadUV[2];
		baseVertex[3].mUV = sprite.mQuadUV[3];
	}
	void AddQuad(const VEntSprite& spriteData, VScene& scene)
	{
		VSprite::Vertex* baseVertex = AddQuad();

		const VSprite& sprite = *spriteData.mSprite;
		InitQuadUV(baseVertex, sprite);

		VVec2F pixelSize = sprite.GetSizeInPixel() * sprite.mDrawScale;

		VVec2F offset = sprite.mOrigin * pixelSize;
		VVec2F location = spriteData.mLocation - offset;

		VVec2F originWS = spriteData.mLocation;

		InitQuadPositions(baseVertex, location, pixelSize);

		for(int i = 0; i < 4; i++)
		{
			baseVertex[i].mPosition.RotateAround(spriteData.mRotation, originWS);
			baseVertex[i].mPosition -= scene.mCamera.mPosition;
		}
		
	}
	VertexType* AddQuad()
	{
		CheckCap();

		auto pBaseVertex = mVertices.Elements() + NumVertex();
		auto pBaseIndex = mIndices.Elements() + mIndexCount;

		IndexType baseVertexIndex = NumVertex();
		if (baseVertexIndex > 0xFFF0)
		{
			assert(false && "index buffer overflow");
			return nullptr;
		}

		pBaseIndex[0] = baseVertexIndex + 0;
		pBaseIndex[1] = baseVertexIndex + 1;
		pBaseIndex[2] = baseVertexIndex + 2;

		pBaseIndex[3] = baseVertexIndex + 1;
		pBaseIndex[4] = baseVertexIndex + 3;
		pBaseIndex[5] = baseVertexIndex + 2;

		mIndexCount += 6;

		return pBaseVertex;
	}

};

//used for batching all the sprites with different texture
struct VSpriteBuilder
{
	static const int MAX_DRAWLIST = 128;

	VScene* mScene = nullptr;

	struct FinalBatchList
	{
		uint32_t mVBOffset;
		uint32_t mIBOffset;
		uint32_t mIndexCount;
	};

	struct SpriteGroup
	{
		VTexture2D*				mTexture;
		VArray<int>				mIndices;
		VSpriteVertexBuilder	mVertexBuilder;
		FinalBatchList			mFinalBatch;

		void Reset()
		{
			mTexture = nullptr;
			mIndices.Reset();
			mVertexBuilder.Reset();
			mFinalBatch = FinalBatchList{0,0,0};
		}
	};

	SpriteGroup mGroups[MAX_DRAWLIST];
	VStackArray<int, MAX_DRAWLIST> mActiveGroups;
	bool mGroupActivated[MAX_DRAWLIST] = {};

	size_t GetTotalVertex() const
	{
		size_t total = 0;
		for(int i = 0; i < mActiveGroups.Length(); i++)
			total += mGroups[mActiveGroups[i]].mVertexBuilder.NumVertex();

		return total;
	}
	size_t GetTotalIndex() const
	{
		size_t total = 0;
		for (int i = 0; i < mActiveGroups.Length(); i++)
			total += mGroups[mActiveGroups[i]].mVertexBuilder.NumIndex();

		return total;
	}
	void Reset()
	{
		for (int i = 0; i < mActiveGroups.Length(); i++)
		{
			SpriteGroup& group = mGroups[mActiveGroups[i]];
			group.Reset();
		}

		mActiveGroups.Reset();

		memset(mGroupActivated, 0, sizeof(mGroupActivated));
	}
	void Sort()
	{
		const auto SortProc = [this](int l, int r) {
			VEntSprite& leftComp = mScene->mSprites.GetItemAt(l);
			VEntSprite& rightComp = mScene->mSprites.GetItemAt(r);
			return leftComp.mDepth > rightComp.mDepth;
		};

		for (int i = 0; i < mActiveGroups.Length(); i++)
		{
			SpriteGroup& group = mGroups[mActiveGroups[i]];
			if (group.mIndices.Length() == 0)
				continue;

			std::sort(group.mIndices.Elements(), group.mIndices.Elements() + group.mIndices.Length(), SortProc);
		}
	}
	void BuildVertices()
	{
		for (int i = 0; i < mActiveGroups.Length(); i++)
		{
			SpriteGroup& group = mGroups[mActiveGroups[i]];

			for (int iSprite = 0; iSprite < group.mIndices.Length(); iSprite++)
			{
				const VEntSprite& spriteData = mScene->mSprites.GetItemAt(group.mIndices[iSprite]);
				group.mVertexBuilder.AddQuad(spriteData, *mScene);
			}
		}
	}
	void FillRendererBuffers(VSpriteRenderer& renderer)
	{
		auto pVertsMapped = (uint8_t*)renderer.mVertexBuffer.Map(GetTotalVertex() * sizeof(VSprite::Vertex), D3D11_MAP_WRITE_DISCARD, 0);
		auto pIndsMapped =  (uint8_t*)renderer.mIndexBuffer.Map(GetTotalIndex() * sizeof(VSpriteVertexBuilder::IndexType), D3D11_MAP_WRITE_DISCARD, 0);

		if (pVertsMapped && pIndsMapped)
		{
			uint32_t vbOffsetInBytes = 0;
			uint32_t ibOffsetInBytes = 0;

			for (int i = 0; i < mActiveGroups.Length(); i++)
			{
				SpriteGroup& group = mGroups[mActiveGroups[i]];
				VSpriteVertexBuilder& builder = group.mVertexBuilder;
				
				if(builder.NumIndex() == 0)
					continue;

				size_t vbChnukSize = builder.NumVertex() * sizeof(VSpriteVertexBuilder::VertexType);
				memcpy(pVertsMapped + vbOffsetInBytes, builder.Vertices(), vbChnukSize);

				size_t ibChunkSize = builder.NumIndex() * sizeof(VSpriteVertexBuilder::IndexType);
				memcpy(pIndsMapped + ibOffsetInBytes, builder.Indices(), ibChunkSize);
				
				group.mFinalBatch = FinalBatchList{ vbOffsetInBytes, ibOffsetInBytes, (uint32_t)builder.NumIndex() };

				vbOffsetInBytes += vbChnukSize;
				ibOffsetInBytes += ibChunkSize;

			}
		}

		renderer.mVertexBuffer.UnMap();
		renderer.mIndexBuffer.UnMap();


	}
	void DrawVertices(VSpriteRenderer& renderer)
	{
		for (int i = 0; i < mActiveGroups.Length(); i++)
		{
			SpriteGroup& group = mGroups[mActiveGroups[i]];
			
			if(group.mFinalBatch.mIndexCount == 0)
				continue;

			DXGI_FORMAT ibFormat = sizeof(VSpriteVertexBuilder::IndexType) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
			gImmediateContext->IASetIndexBuffer(renderer.mIndexBuffer.mBuffer.Get(), ibFormat, group.mFinalBatch.mIBOffset);

			UINT strides[1] = { sizeof(VSpriteVertexBuilder::VertexType) };
			UINT offsets[1] = { group.mFinalBatch.mVBOffset };
			ID3D11Buffer* buffers[1] = { renderer.mVertexBuffer.mBuffer.Get() };

			gImmediateContext->IASetVertexBuffers(0, 1, buffers, strides, offsets);
			
			gImmediateContext->PSSetSamplers(0, 1, renderer.DefaultSampler.GetAddressOf());
			gImmediateContext->PSSetShaderResources(0, 1, group.mTexture->mDefaultView.GetAddressOf());

			gImmediateContext->DrawIndexed(group.mFinalBatch.mIndexCount, 0, 0);
		}
	}
};


VScene::VScene()
{
	mLastBuilder = std::make_unique<VSpriteBuilder>();
	mLastBuilder->mScene = this;
}



VEntSprite& VScene::AddSprite()
{
	return mSprites.Add();
}

void VScene::MakeSpriteBatch()
{
	VSpriteBuilder& builder = *mLastBuilder;
	builder.Reset();

	for (int iSprite = 0; iSprite < mSprites.Length(); iSprite++)
	{
		//zero hash means the slot is empty
		if(mSprites.GetHashAt(iSprite) == 0)
			continue;

		VEntSprite& comp = mSprites.GetItemAt(iSprite);
		if (comp.mSprite && comp.mSprite->mTexture && !comp.mHiddenInGame)
		{
			//we batch sprites with same texture (its atlas) as one draw call
			const int textureId = comp.mSprite->mTexture->GetTextureIndex();
			assert(textureId != 0);

			VSpriteBuilder::SpriteGroup& group = builder.mGroups[textureId];

			const bool bActivated = builder.mGroupActivated[textureId];
			if (!bActivated)
			{
				builder.mGroupActivated[textureId] = true;
				builder.mActiveGroups.Add(textureId);

				group.mTexture = comp.mSprite->mTexture;
			}
			
			group.mIndices.AddEmplace(iSprite);
		}
	}

	builder.Sort();
	builder.BuildVertices();


}

void VScene::Render()
{
	MakeSpriteBatch();


	mLastBuilder->FillRendererBuffers(gEngine->SpriteRenderer);
	mLastBuilder->DrawVertices(gEngine->SpriteRenderer);
	
	
}

VVec2F GetArrowAxisValue()
{
	VVec2F input(0);

	if (ImGui::GetIO().KeysDown[VK_RIGHT])
		input.X += 1;
	if (ImGui::GetIO().KeysDown[VK_LEFT])
		input.X -= 1;

	if (ImGui::GetIO().KeysDown[VK_UP])
		input.Y += 1;
	if (ImGui::GetIO().KeysDown[VK_DOWN])
		input.Y -= 1;

	return input;

}
void VScene::Tick(float delta)
{
	if (ImGui::GetIO().MouseDown[1])
	{
		VEntSprite& sprite = AddSprite();
		sprite.mLocation = VVec2F(ImGui::GetIO().MousePos) + mCamera.mPosition;
		sprite.mSprite = VGameplayConfig::Get().mTestSprite; 
		sprite.mRotation = rand() / 100.0f;
	}

	if (ImGui::IsKeyReleased(VK_F1))
	{
		gEngine->SpriteRenderer.mDrawWireFrame = !gEngine->SpriteRenderer.mDrawWireFrame;
	}

	
	mCamera.mPosition = mCamera.mPosition + GetArrowAxisValue() * VGameplayConfig::Get().mCamMoveSpeed * delta;
}

void VScene::InitPhysx()
{
	mPhysSpace = cpSpaceNew();
	cpSpaceSetIterations(mPhysSpace, 30);
	cpSpaceSetUserData(mPhysSpace, this);
}

void VScene::DestroyPhysx()
{
	cpSpaceFree(mPhysSpace);
}

void VScene::TickPhys()
{
	cpSpaceStep(mPhysSpace, 1.0f / 60.0f);
}

