#include "Matrix.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    DirectX::SimpleMath::Matrix create_billboard(
        const DirectX::SimpleMath::Vector3& position,
        const DirectX::SimpleMath::Matrix& offset,
        const DirectX::SimpleMath::Matrix& scale,
        const ICamera& camera)
    {
        Vector3 forward = camera.forward();
        return scale * Matrix::CreateBillboard(position, camera.position(), camera.up(), &forward) * offset;
    }

    DirectX::SimpleMath::Matrix create_billboard(
        const DirectX::SimpleMath::Vector3& position,
        const DirectX::SimpleMath::Vector3& offset,
        const DirectX::SimpleMath::Matrix& scale,
        const ICamera& camera)
    {
        return create_billboard(position, Matrix::CreateTranslation(offset), scale, camera);
    }
}
