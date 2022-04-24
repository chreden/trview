#include "PickFilter.h"

namespace trview
{
    PickFilter filter_flag(PickFilter filter, bool condition)
    {
        return condition ? filter : PickFilter::None;
    }
}
