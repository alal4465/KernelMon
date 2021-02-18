#pragma once

namespace arch {
    enum class MtrrType : unsigned __int64
    {
        UC = 0,
        WC = 1,
        WT = 4,
        WP = 5,
        WB = 6
    };
}