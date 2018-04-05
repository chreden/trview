#pragma once

namespace trview
{
    // Collects transparent triangles to be rendered and provides
    // the buffers required for rendering.
    class TransparencyBuffer
    {
    public:
        TransparencyBuffer() = default;
        TransparencyBuffer(const TransparencyBuffer&) = delete;
        TransparencyBuffer& operator=(const TransparencyBuffer&) = delete;

        // Sort the accumulated transparent triangles in order of farthest to
        // nearest, based on the position of the camera.
        void sort();
    };
}
