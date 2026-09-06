export module trview.app:Picking;

import trview.common;

import :IPicking;

namespace trview
{
    export class Picking final : public IPicking
    {
    public:
        virtual ~Picking() = default;

        explicit Picking(const Window& window);

        /// <summary>
        /// Perform a pick operation.
        /// </summary>
        /// <param name="camera">The current scene camera.</param>
        virtual void pick(const ICamera& camera) override;
    private:
        Window _window;
    };
}
