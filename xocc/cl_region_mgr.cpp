/*@
Copyright (c) 2013-2021, Su Zhenyu steven.known@gmail.com

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Su Zhenyu nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

author: Su Zhenyu
@*/
#include "xoccinc.h"

namespace xocc {

VarMgr * CLRegionMgr::allocVarMgr()
{
    return new CLVarMgr(this);
}


Region * CLRegionMgr::allocRegion(REGION_TYPE rt)
{
    return new CLRegion(rt, this);
}


bool CLRegionMgr::compileFuncRegion(xoc::Region * func, xgen::CGMgr * cgmgr,
                                    xoc::OptCtx * oc)
{
    ASSERT0(func && cgmgr && oc);
    ASSERT0(func->is_function() || func->is_program() || func->is_inner());
    START_TIMER_FMT(t, ("compileFuncRegion '%s'", func->getRegionName()));
    //Note we regard INNER region as FUNCTION region.
    if (!xoc::RegionMgr::processFuncRegion(func, oc)) {
        return false;
    }
    if (xgen::g_do_cg) {
        cgmgr->generate(func);
        if (g_dump_opt.isDumpMemUsage()) {
            func->dumpMemUsage();
        }
    }
    END_TIMER_FMT(t, ("compileFuncRegion '%s'", func->getRegionName()));
    return true;
}


CLRegionMgr * allocCLRegionMgr()
{
    return new CLRegionMgr();
}

} //namespace xocc