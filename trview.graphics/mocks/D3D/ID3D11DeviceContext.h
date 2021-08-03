#pragma once

#include <d3d11.h>

namespace trview
{
    namespace graphics
    {
        namespace mocks
        {
            struct MockD3D11DeviceContext final : public ID3D11DeviceContext
            {
                virtual ~MockD3D11DeviceContext() = default;

                // IUnknown
                MOCK_METHOD(HRESULT, QueryInterface, (REFIID riid, void** ppvObject), (override));
                unsigned int ref_count{ 1 };

                virtual ULONG AddRef() override
                {
                    InterlockedIncrement(&ref_count);
                    return ref_count;
                }

                virtual ULONG Release() override
                {
                    ULONG ulRefCount = InterlockedDecrement(&ref_count);
                    if (0 == ref_count)
                    {
                        delete this;
                    }
                    return ulRefCount;
                }

                template<class Q>
                HRESULT QueryInterface(_COM_Outptr_ Q** pp)
                {
                    return QueryInterface(__uuidof(Q), (void**)pp);
                }

                // ID3D11DeviceChild
                MOCK_METHOD(void, GetDevice, (ID3D11Device** ppDevice), (override));
                MOCK_METHOD(HRESULT, GetPrivateData, (REFGUID guid, UINT* pDataSize, void* pData), (override));
                MOCK_METHOD(HRESULT, SetPrivateData, (REFGUID guid, UINT DataSize, const void* pData), (override));
                MOCK_METHOD(HRESULT, SetPrivateDataInterface, (REFGUID guid, const IUnknown* pData), (override));
                // ID3D11DeviceContext
                MOCK_METHOD(void, VSSetConstantBuffers, (UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers), (override));
                MOCK_METHOD(void, PSSetShaderResources, (UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews), (override));
                MOCK_METHOD(void, PSSetShader, (ID3D11PixelShader* pPixelShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances), (override));
                MOCK_METHOD(void, PSSetSamplers, (UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers), (override));
                MOCK_METHOD(void, VSSetShader, (ID3D11VertexShader* pVertexShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances), (override));
                MOCK_METHOD(void, DrawIndexed, (UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation), (override));
                MOCK_METHOD(void, Draw, (UINT VertexCount, UINT StartVertexLocation), (override));
                MOCK_METHOD(HRESULT, Map, (ID3D11Resource* pResource, UINT Subresource, D3D11_MAP MapType, UINT MapFlags, D3D11_MAPPED_SUBRESOURCE* pMappedResource), (override));
                MOCK_METHOD(void, Unmap, (ID3D11Resource* pResource, UINT Subresource), (override));
                MOCK_METHOD(void, PSSetConstantBuffers, (UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers), (override));
                MOCK_METHOD(void, IASetInputLayout, (ID3D11InputLayout* pInputLayout), (override));
                MOCK_METHOD(void, IASetVertexBuffers, (UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppVertexBuffers, const UINT* pStrides, const UINT* pOffsets), (override));
                MOCK_METHOD(void, IASetIndexBuffer, (ID3D11Buffer* pIndexBuffer, DXGI_FORMAT Format, UINT Offset), (override));
                MOCK_METHOD(void, DrawIndexedInstanced, (UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation), (override));
                MOCK_METHOD(void, DrawInstanced, (UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation), (override));
                MOCK_METHOD(void, GSSetConstantBuffers, (UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers), (override));
                MOCK_METHOD(void, GSSetShader, (ID3D11GeometryShader* pShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances), (override));
                MOCK_METHOD(void, IASetPrimitiveTopology, (D3D11_PRIMITIVE_TOPOLOGY Topology), (override));
                MOCK_METHOD(void, VSSetShaderResources, (UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews), (override));
                MOCK_METHOD(void, VSSetSamplers, (UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers), (override));
                MOCK_METHOD(void, Begin, (ID3D11Asynchronous* pAsync), (override));
                MOCK_METHOD(void, End, (ID3D11Asynchronous* pAsync), (override));
                MOCK_METHOD(HRESULT, GetData, (ID3D11Asynchronous* pAsync, void* pData, UINT DataSize, UINT GetDataFlags), (override));
                MOCK_METHOD(void, SetPredication, (ID3D11Predicate* pPredicate, BOOL PredicateValue), (override));
                MOCK_METHOD(void, GSSetShaderResources, (UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews), (override));
                MOCK_METHOD(void, GSSetSamplers, (UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers), (override));
                MOCK_METHOD(void, OMSetRenderTargets, (UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView), (override));
                MOCK_METHOD(void, OMSetRenderTargetsAndUnorderedAccessViews, (UINT NumRTVs, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView, UINT UAVStartSlot, UINT NumUAVs, ID3D11UnorderedAccessView* const* ppUnorderedAccessViews, const UINT* pUAVInitialCounts), (override));
                MOCK_METHOD(void, OMSetBlendState, (ID3D11BlendState* pBlendState, const FLOAT BlendFactor[4], UINT SampleMask), (override));
                MOCK_METHOD(void, OMSetDepthStencilState, (ID3D11DepthStencilState* pDepthStencilState, UINT StencilRef), (override));
                MOCK_METHOD(void, SOSetTargets, (UINT NumBuffers, ID3D11Buffer* const* ppSOTargets, const UINT* pOffsets), (override));
                MOCK_METHOD(void, DrawAuto, (), (override));
                MOCK_METHOD(void, DrawIndexedInstancedIndirect, (ID3D11Buffer* pBufferForArgs, UINT AlignedByteOffsetForArgs), (override));
                MOCK_METHOD(void, DrawInstancedIndirect, (ID3D11Buffer* pBufferForArgs, UINT AlignedByteOffsetForArgs), (override));
                MOCK_METHOD(void, Dispatch, (UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ), (override));
                MOCK_METHOD(void, DispatchIndirect, (ID3D11Buffer* pBufferForArgs, UINT AlignedByteOffsetForArgs), (override));
                MOCK_METHOD(void, RSSetState, (ID3D11RasterizerState* pRasterizerState), (override));
                MOCK_METHOD(void, RSSetViewports, (UINT NumViewports, const D3D11_VIEWPORT* pViewports), (override));
                MOCK_METHOD(void, RSSetScissorRects, (UINT NumRects, const D3D11_RECT* pRects), (override));
                MOCK_METHOD(void, CopySubresourceRegion, (ID3D11Resource* pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, ID3D11Resource* pSrcResource, UINT SrcSubresource, const D3D11_BOX* pSrcBox), (override));
                MOCK_METHOD(void, CopyResource, (ID3D11Resource* pDstResource, ID3D11Resource* pSrcResource), (override));
                MOCK_METHOD(void, UpdateSubresource, (ID3D11Resource* pDstResource, UINT DstSubresource, const D3D11_BOX* pDstBox, const void* pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch), (override));
                MOCK_METHOD(void, CopyStructureCount, (ID3D11Buffer* pDstBuffer, UINT DstAlignedByteOffset, ID3D11UnorderedAccessView* pSrcView), (override));
                MOCK_METHOD(void, ClearRenderTargetView, (ID3D11RenderTargetView* pRenderTargetView, const FLOAT ColorRGBA[4]), (override));
                MOCK_METHOD(void, ClearUnorderedAccessViewUint, (ID3D11UnorderedAccessView* pUnorderedAccessView, const UINT Values[4]), (override));
                MOCK_METHOD(void, ClearUnorderedAccessViewFloat, (ID3D11UnorderedAccessView* pUnorderedAccessView, const FLOAT Values[4]), (override));
                MOCK_METHOD(void, ClearDepthStencilView, (ID3D11DepthStencilView* pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil), (override));
                MOCK_METHOD(void, GenerateMips, (ID3D11ShaderResourceView* pShaderResourceView), (override));
                MOCK_METHOD(void, SetResourceMinLOD, (ID3D11Resource* pResource, FLOAT MinLOD), (override));
                MOCK_METHOD(FLOAT, GetResourceMinLOD, (ID3D11Resource* pResource), (override));
                MOCK_METHOD(void, ResolveSubresource, (ID3D11Resource* pDstResource, UINT DstSubresource, ID3D11Resource* pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format), (override));
                MOCK_METHOD(void, ExecuteCommandList, (ID3D11CommandList* pCommandList, BOOL RestoreContextState), (override));
                MOCK_METHOD(void, HSSetShaderResources, (UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews), (override));
                MOCK_METHOD(void, HSSetShader, (ID3D11HullShader* pHullShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances), (override));
                MOCK_METHOD(void, HSSetSamplers, (UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers), (override));
                MOCK_METHOD(void, HSSetConstantBuffers, (UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers), (override));
                MOCK_METHOD(void, DSSetShaderResources, (UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews), (override));
                MOCK_METHOD(void, DSSetShader, (ID3D11DomainShader* pDomainShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances), (override));
                MOCK_METHOD(void, DSSetSamplers, (UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers), (override));
                MOCK_METHOD(void, DSSetConstantBuffers, (UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers), (override));
                MOCK_METHOD(void, CSSetShaderResources, (UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews), (override));
                MOCK_METHOD(void, CSSetUnorderedAccessViews, (UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView* const* ppUnorderedAccessViews, const UINT* pUAVInitialCounts), (override));
                MOCK_METHOD(void, CSSetShader, (ID3D11ComputeShader* pComputeShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances), (override));
                MOCK_METHOD(void, CSSetSamplers, (UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers), (override));
                MOCK_METHOD(void, CSSetConstantBuffers, (UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers), (override));
                MOCK_METHOD(void, VSGetConstantBuffers, (UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers), (override));
                MOCK_METHOD(void, PSGetShaderResources, (UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews), (override));
                MOCK_METHOD(void, PSGetShader, (ID3D11PixelShader** ppPixelShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances), (override));
                MOCK_METHOD(void, PSGetSamplers, (UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers), (override));
                MOCK_METHOD(void, VSGetShader, (ID3D11VertexShader** ppVertexShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances), (override));
                MOCK_METHOD(void, PSGetConstantBuffers, (UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers), (override));
                MOCK_METHOD(void, IAGetInputLayout, (ID3D11InputLayout** ppInputLayout), (override));
                MOCK_METHOD(void, IAGetVertexBuffers, (UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppVertexBuffers, UINT* pStrides, UINT* pOffsets), (override));
                MOCK_METHOD(void, IAGetIndexBuffer, (ID3D11Buffer** pIndexBuffer, DXGI_FORMAT* Format, UINT* Offset), (override));
                MOCK_METHOD(void, GSGetConstantBuffers, (UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers), (override));
                MOCK_METHOD(void, GSGetShader, (ID3D11GeometryShader** ppGeometryShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances), (override));
                MOCK_METHOD(void, IAGetPrimitiveTopology, (D3D11_PRIMITIVE_TOPOLOGY* pTopology), (override));
                MOCK_METHOD(void, VSGetShaderResources, (UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews), (override));
                MOCK_METHOD(void, VSGetSamplers, (UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers), (override));
                MOCK_METHOD(void, GetPredication, (ID3D11Predicate** ppPredicate, BOOL* pPredicateValue), (override));
                MOCK_METHOD(void, GSGetShaderResources, (UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews), (override));
                MOCK_METHOD(void, GSGetSamplers, (UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers), (override));
                MOCK_METHOD(void, OMGetRenderTargets, (UINT NumViews, ID3D11RenderTargetView** ppRenderTargetViews, ID3D11DepthStencilView** ppDepthStencilView), (override));
                MOCK_METHOD(void, OMGetRenderTargetsAndUnorderedAccessViews, (UINT NumRTVs, ID3D11RenderTargetView** ppRenderTargetViews, ID3D11DepthStencilView** ppDepthStencilView, UINT UAVStartSlot, UINT NumUAVs, ID3D11UnorderedAccessView** ppUnorderedAccessViews), (override));
                MOCK_METHOD(void, OMGetBlendState, (ID3D11BlendState** ppBlendState, FLOAT BlendFactor[4], UINT* pSampleMask), (override));
                MOCK_METHOD(void, OMGetDepthStencilState, (ID3D11DepthStencilState** ppDepthStencilState, UINT* pStencilRef), (override));
                MOCK_METHOD(void, SOGetTargets, (UINT NumBuffers, ID3D11Buffer** ppSOTargets), (override));
                MOCK_METHOD(void, RSGetState, (ID3D11RasterizerState** ppRasterizerState), (override));
                MOCK_METHOD(void, RSGetViewports, (UINT* pNumViewports, D3D11_VIEWPORT* pViewports), (override));
                MOCK_METHOD(void, RSGetScissorRects, (UINT* pNumRects, D3D11_RECT* pRects), (override));
                MOCK_METHOD(void, HSGetShaderResources, (UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews), (override));
                MOCK_METHOD(void, HSGetShader, (ID3D11HullShader** ppHullShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances), (override));
                MOCK_METHOD(void, HSGetSamplers, (UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers), (override));
                MOCK_METHOD(void, HSGetConstantBuffers, (UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers), (override));
                MOCK_METHOD(void, DSGetShaderResources, (UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews), (override));
                MOCK_METHOD(void, DSGetShader, (ID3D11DomainShader** ppDomainShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances), (override));
                MOCK_METHOD(void, DSGetSamplers, (UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers), (override));
                MOCK_METHOD(void, DSGetConstantBuffers, (UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers), (override));
                MOCK_METHOD(void, CSGetShaderResources, (UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews), (override));
                MOCK_METHOD(void, CSGetUnorderedAccessViews, (UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView** ppUnorderedAccessViews), (override));
                MOCK_METHOD(void, CSGetShader, (ID3D11ComputeShader** ppComputeShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances), (override));
                MOCK_METHOD(void, CSGetSamplers, (UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers), (override));
                MOCK_METHOD(void, CSGetConstantBuffers, (UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers), (override));
                MOCK_METHOD(void, ClearState, (), (override));
                MOCK_METHOD(void, Flush, (), (override));
                MOCK_METHOD(D3D11_DEVICE_CONTEXT_TYPE, GetType, (), (override));
                MOCK_METHOD(UINT, GetContextFlags, (), (override));
                MOCK_METHOD(HRESULT, FinishCommandList, (BOOL RestoreDeferredContextState, ID3D11CommandList** ppCommandList), (override));
            };
        }
    }
}