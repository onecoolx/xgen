/*@
Copyright (c) 2013-2014, Su Zhenyu steven.known@gmail.com

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
@*/

#include "../opt/cominc.h"
#include "feinc.h"
#include "../xgen/xgeninc.h"
#include "errno.h"
#include "../opt/util.h"
#include "../reader/grreader.h"
#include "../opt/comopt.h"

CHAR const* g_output_file_name = nullptr;
CHAR const* g_xocc_version = "1.0.0";
CHAR const* g_c_file_name = nullptr;
CHAR const* g_gr_file_name = nullptr;
CHAR const* g_dump_file_name = nullptr;
bool g_is_dumpgr = false;

static bool is_c_source_file(CHAR const* fn)
{
    UINT len = (UINT)strlen(fn) + 1;
    CHAR * buf = (CHAR*)ALLOCA(len);
    upper(getfilesuffix(fn, buf, (UINT)len));
    if (strcmp(buf, "C") == 0 || strcmp(buf, "I") == 0) {
        return true;
    }
    return false;
}


static bool is_gr_source_file(CHAR const* fn)
{
    UINT len = (UINT)strlen(fn) + 2;
    CHAR * buf = (CHAR*)ALLOCA(len);
    upper(getfilesuffix(fn, buf, len));
    if (strcmp(buf, "GR") == 0 || strcmp(buf, "I") == 0) {
        return true;
    }
    return false;
}


static bool process_optimize(INT argc, CHAR const* argv[], INT & i)
{
    DUMMYUSE(argc);
    CHAR const* cmdstr = &argv[i][1];
    switch (cmdstr[1]) {
    case '0':
        xoc::g_opt_level = OPT_LEVEL0;
        break;
    case '1':
        xoc::g_opt_level = OPT_LEVEL1;
        xoc::g_do_pr_ssa = true;
        xoc::g_do_md_ssa = true;
        //Only do refinement.
        break;
    case '2':
        xoc::g_opt_level = OPT_LEVEL2;        
        xoc::g_do_dce = true;
        xoc::g_do_licm = true;
        xoc::g_do_rp = true;
        xoc::g_do_pr_ssa = true;
        xoc::g_do_md_ssa = true;
        break;    
    case '3':
        xoc::g_opt_level = OPT_LEVEL3;
        xoc::g_do_dce = true;
        xoc::g_do_licm = true;
        xoc::g_do_rp = true;
        xoc::g_do_pr_ssa = true;
        xoc::g_do_md_ssa = true;
        break;
    case 's':
    case 'S':
        xoc::g_opt_level = SIZE_OPT;
        xoc::g_do_dce = true;
        xoc::g_do_licm = true;
        xoc::g_do_rp = true;
        xoc::g_do_pr_ssa = true;
        xoc::g_do_md_ssa = true;
        break;
    default:
        xoc::g_opt_level = OPT_LEVEL1;
        xoc::g_do_pr_ssa = true;
        xoc::g_do_md_ssa = true;
    }
    i++;
    return true;
}


static bool process_output_file(INT argc, CHAR const* argv[], INT & i)
{
    if (i + 1 < argc && argv[i + 1] != nullptr) {
        g_output_file_name = argv[i + 1];
    }
    i += 2;
    return true;
}



static CHAR const* process_dump(INT argc, CHAR const* argv[], INT & i)
{
    CHAR const* n = nullptr;
    if (i + 1 < argc && argv[i + 1] != nullptr) {
        n = argv[i + 1];
    }
    i += 2;
    return n;
}


class BoolOption {
protected:
    class Desc {
    public:
        CHAR const* name;
        bool * option;
        CHAR const* intro;
    };

    static Desc const option_desc[];
    static Desc const dump_option_desc[];
    static CHAR const* dump_option_prefix;
    static CHAR const* disable_option_prefix;

public:
    static UINT getNumOfOption();
    static UINT getNumOfDumpOption();

    static bool is_option(CHAR const* str, bool ** option)
    {
        ASSERT0(option);
        for (UINT i = 0; i < BoolOption::getNumOfOption(); i++) {
            if (strcmp(BoolOption::option_desc[i].name, str) == 0) {
                *option = BoolOption::option_desc[i].option;
                return true;
            }
        }
        return false;
    }

    static bool is_dump_option(CHAR const* str, bool ** option)
    {
        ASSERT0(option);
        for (UINT i = 0; i < BoolOption::getNumOfDumpOption(); i++) {
            if (strcmp(BoolOption::dump_option_desc[i].name, str) == 0) {
                *option = BoolOption::dump_option_desc[i].option;
                return true;
            }
        }
        return false;
    }

    static void dump_usage(StrBuf & output)
    {
        UINT max_name_len = 0;
        for (UINT i = 0; i < BoolOption::getNumOfOption(); i++) {
            max_name_len = MAX((UINT)strlen(BoolOption::option_desc[i].name),
                               max_name_len);
        }
        UINT alignbuflen = max_name_len + 2;

        CHAR * alignbuf = (CHAR*)ALLOCA(alignbuflen);
        for (UINT i = 0; i < BoolOption::getNumOfOption(); i++) {
            UINT j = 0;
            for (; j < strlen(BoolOption::option_desc[i].name); j++) {
                alignbuf[j] = BoolOption::option_desc[i].name[j];
            }
            for (; j < alignbuflen - 1; j++) {
                alignbuf[j] = ' '; //pad the rest with blank
            }
            alignbuf[j] = 0;

            output.strcat("\n -%s %s", alignbuf, 
                          BoolOption::option_desc[i].intro);
        }
    }

    static void dump_dump_option(StrBuf & output)
    {
        UINT max_name_len = 0;
        for (UINT i = 0; i < BoolOption::getNumOfDumpOption(); i++) {
            max_name_len = MAX((UINT)strlen(BoolOption::dump_option_desc[i].name),
                               max_name_len);
        }
        UINT alignbuflen = max_name_len + 2;

        CHAR * alignbuf = (CHAR*)ALLOCA(alignbuflen);
        for (UINT i = 0; i < BoolOption::getNumOfOption(); i++) {
            UINT j = 0;
            for (; j < strlen(BoolOption::dump_option_desc[i].name); j++) {
                alignbuf[j] = BoolOption::dump_option_desc[i].name[j];
            }
            for (; j < alignbuflen - 1; j++) {
                alignbuf[j] = ' '; //pad the rest with blank
            }
            alignbuf[j] = 0;

            output.strcat("\n -%s%s %s", BoolOption::dump_option_prefix,
                          alignbuf, BoolOption::dump_option_desc[i].intro);
        }
    }
};

CHAR const* BoolOption::dump_option_prefix = "dump-";
CHAR const* BoolOption::disable_option_prefix = "no-";

BoolOption::Desc const BoolOption::option_desc[] = {
    { "time", &xoc::g_show_time,
      "show compilation time", },
    { "licm", &xoc::g_do_licm,
      "enable loop-invariant-code-motion optimization", },
    { "rp", &xoc::g_do_rp,
      "enable register-promotion optimization", },
    { "cp", &xoc::g_do_cp,
      "enable copy-propagation optimization", },
    { "rce", &xoc::g_do_rce,
      "enable redundant-code-elimination optimization", },
    { "dce", &xoc::g_do_dce,
      "enable aggressive-dead-code-elimination optimization", },
    { "mdssa", &xoc::g_do_md_ssa,
      "enable md-ssa analysis", },
    { "prssa", &xoc::g_do_pr_ssa,
      "enable pr-ssa analysis", },
    { "prmode", &xoc::g_is_lower_to_pr_mode,
      "lowering cascading IR mode to pr-transfering mode", },
    { "prdu", &xoc::g_compute_pr_du_chain,
      "enable classic PR def-use analysis", },
    { "nonprdu", &xoc::g_compute_nonpr_du_chain,
      "enable classic NON-PR def-use analysis", },
    { "redirect_stdout_to_dump_file", &xoc::g_redirect_stdout_to_dump_file,
      "redirect internal compiler output information to given dump file", },
    { "ipa", &xoc::g_do_ipa,
      "enable interprocedual analysis", },
    { "schedule_delay_slot", &xgen::g_enable_schedule_delay_slot,
      "enable scheduling branch-delay-slot", },
    { "cg", &xgen::g_do_cg,
      "enable target code generation", },
    { "gra", &xgen::g_do_gra,
      "enable global-register-allocation", },
};


BoolOption::Desc const BoolOption::dump_option_desc[] = {
    { "cfg", &xoc::g_dump_opt.is_dump_cfg,
      "dump control-flow-graph", },
    { "lis", &xoc::g_dump_opt.is_dump_lis,
      "dump instruction-scheduling", },
    { "aa", &xoc::g_dump_opt.is_dump_aa,
      "dump alias-analysis", },
    { "dce", &xoc::g_dump_opt.is_dump_dce,
      "dump dead-code-elimination", },
    { "rp", &xoc::g_dump_opt.is_dump_rp,
      "dump register-promotion", },
    { "licm", &xoc::g_dump_opt.is_dump_licm,
      "dump loop-invariant-code-motion", },
    { "dumgr", &xoc::g_dump_opt.is_dump_dumgr,
      "dump classic def-use information", },
    { "prssa", &xoc::g_dump_opt.is_dump_prssamgr,
      "dump pr-ssa information", },
    { "mdssa", &xoc::g_dump_opt.is_dump_mdssamgr,
      "dump md-ssa information", },
    { "rce", &xoc::g_dump_opt.is_dump_rce,
      "dump redundant-code-elimination", },
    { "ra", &xoc::g_dump_opt.is_dump_ra,
      "dump register allocation", },
    { "cg", &xoc::g_dump_opt.is_dump_cg,
      "dump target code generation", },
    { "cdg", &xoc::g_dump_opt.is_dump_cdg,
      "dump control-dependent-graph", },
    { "simplification", &xoc::g_dump_opt.is_dump_simplification,
      "dump IR simplification", },
    { "refine-duchain", &xoc::g_dump_opt.is_dump_refine_duchain,
      "dump refine-duchain optimization", },
    { "gvn", &xoc::g_dump_opt.is_dump_gvn,
      "dump global-value-numbering", },
    { "all", &xoc::g_dump_opt.is_dump_all,
      "dump all compiler information", },
    { "nothing", &xoc::g_dump_opt.is_dump_nothing,
      "disable dump", },
    { "gr", &g_is_dumpgr,
      "output GR language according region information", },
};
 

UINT BoolOption::getNumOfOption()
{
    return sizeof(BoolOption::option_desc) /
           sizeof(BoolOption::option_desc[0]);
}


UINT BoolOption::getNumOfDumpOption()
{
    return sizeof(BoolOption::dump_option_desc) /
           sizeof(BoolOption::dump_option_desc[0]);
}

class IntOption {
public:
    class Desc {
    public:
        CHAR const* name;
        INT * option;
        CHAR const* intro;
    };
    static Desc const option_desc[];
    static UINT getNumOfOption();

    static bool is_option(CHAR const* str, INT ** option)
    {
        ASSERT0(option);
        for (UINT i = 0; i < IntOption::getNumOfOption(); i++) {
            if (strcmp(IntOption::option_desc[i].name, str) == 0) {
                *option = IntOption::option_desc[i].option;
                return true;
            }
        }
        return false;
    }

    static void dump_usage(StrBuf & output)
    {
        UINT max_name_len = 0;
        for (UINT i = 0; i < IntOption::getNumOfOption(); i++) {
            max_name_len = MAX((UINT)strlen(IntOption::option_desc[i].name),
                               max_name_len);
        }
        UINT alignbuflen = max_name_len + 2;

        CHAR * alignbuf = (CHAR*)ALLOCA(alignbuflen);
        for (UINT i = 0; i < IntOption::getNumOfOption(); i++) {
            UINT j = 0;
            for (; j < strlen(IntOption::option_desc[i].name); j++) {
                alignbuf[j] = IntOption::option_desc[i].name[j];
            }
            for (; j < alignbuflen - 1; j++) {
                alignbuf[j] = ' '; //pad the rest with blank
            }
            alignbuf[j] = 0;

            output.strcat("\n -%s = <integer> %s", alignbuf, 
                          IntOption::option_desc[i].intro);
        }
    }

};


IntOption::Desc const IntOption::option_desc[] = {
    { "thres_opt_ir_num", (INT*)&xoc::g_thres_opt_ir_num,
       "the maximum limit of the number of IR to perform optimizations", },
    { "thres_opt_bb_num", (INT*)&xoc::g_thres_opt_bb_num,
       "the maximum limit of the number of IRBB to perform optimizations", }
};


UINT IntOption::getNumOfOption()
{
    return sizeof(IntOption::option_desc) /
           sizeof(IntOption::option_desc[0]);
}


static bool dispatchByPrefixDump(CHAR const* cmdstr, INT argc,
                                 CHAR const* argv[],
                                 INT & i)
{
    bool * boption = nullptr;
    if (BoolOption::is_dump_option(cmdstr, &boption)) {
        ASSERT0(boption);
        *boption = true;
        i++;
        return true;
    }
    return false;
}


static bool dispatchByPrefix(CHAR const* cmdstr, INT argc, CHAR const* argv[],
                             INT & i, bool ** boption)
{
    switch (cmdstr[0]) {
    case 'O': return process_optimize(argc, argv, i);
    case 'o': return process_output_file(argc, argv, i);
    default:
        if (!strcmp(cmdstr, "dump")) {
            CHAR const* n = process_dump(argc, argv, i);
            if (n == nullptr) {
                return false;
            }
            g_dump_file_name = n;
            return true;
        }
    }

    ASSERT0(boption);

    CHAR const* prefix = "no-";
    if (xstrcmp(cmdstr, prefix, (INT)strlen(prefix))) {
        bool res = dispatchByPrefix(cmdstr + strlen(prefix), argc, argv,
                                    i, boption);
        ASSERT0(*boption);
        **boption = !(**boption);
        return res;
    }

    prefix = "dump-";
    if (xstrcmp(cmdstr, prefix, (INT)strlen(prefix))) {
        return dispatchByPrefixDump(cmdstr + strlen(prefix), argc, argv, i);
    }

    if (BoolOption::is_option(cmdstr, boption)) {
        ASSERT0(*boption);
        **boption = true;
        i++;
        return true;
    }

    INT * int_option = nullptr;
    if (IntOption::is_option(cmdstr, &int_option)) {
        ASSERT0(int_option);
        CHAR const* n = nullptr;
        if (i + 1 < argc && argv[i + 1] != nullptr) {
            n = argv[i + 1];
        }
        if (n == nullptr) { return false; } 
        *int_option = (INT)xcom::xatoll(n, false);
        i += 2;
        return true;
    }

    return false;
}


static void usage()
{
    StrBuf buf(128);
    StrBuf buf2(128);
    StrBuf buf3(128);
    BoolOption::dump_usage(buf);
    IntOption::dump_usage(buf2);
    BoolOption::dump_dump_option(buf3);

    fprintf(stdout,
    "\nXOCC Version %s"
    "\nUsage: xocc [options] file"
    "\noptions: "
    "\n -O0,           compile without any optimization"
    "\n -O1,-O2,-O3    compile with optimizations"
    "\n -dump <file>   create dump file"
    "\n -o <file>      output file name"
    "\n -readgr <file> read GR file"
    "\n -no-<option>   disable option, e.g:-no-dce"
    "\n -dump-<option> dump information about option, e.g:-dump-dce"
    "\n"
    "\noption: %s%s"
    "\n"
    "\navailable dump option: %s"
    "\n", g_xocc_version, buf.buf, buf2.buf, buf3.buf);
}


bool processCmdLine(INT argc, CHAR const* argv[])
{
    if (argc <= 1) { usage(); return false; }

    xcom::List<CHAR const*> cfilelist;
    xcom::List<CHAR const*> grfilelist;
    for (INT i = 1; i < argc;) {
        if (argv[i][0] == '-') {
            bool * boption = nullptr;
            if (!dispatchByPrefix(&argv[i][1], argc, argv, i, &boption)) {
                fprintf(stdout, "\nunknown option:%s\n", argv[i]);
                usage();
                return false; 
            }
            continue;
        }

        if (is_c_source_file(argv[i])) {
            cfilelist.append_tail(argv[i]);
            i++;
            continue;
        }

        if (is_gr_source_file(argv[i])) {
            grfilelist.append_tail(argv[i]);
            i++;
            continue;
        }

        //Unknown option.
        usage();
        return false;
    }

    //TDOO: compile all files in list at once.
    g_c_file_name = cfilelist.get_tail();
    g_gr_file_name = grfilelist.get_tail();

    if (g_c_file_name != nullptr) {
        g_hsrc = fopen(g_c_file_name, "rb");
        if (g_hsrc == nullptr) {
            fprintf(stdout, "xoc: cannot open %s, error information is %s\n",
                            g_c_file_name, strerror(errno));
            return false;
        }
    }
    if (g_output_file_name != nullptr) {
        UNLINK(g_output_file_name);
    }
    if (g_opt_level == OPT_LEVEL0) {
        g_do_cfg_remove_empty_bb = false;
        g_do_cfg_remove_unreach_bb = false;
        g_do_cfg_remove_trampolin_bb = false;
        g_do_cfg_remove_unreach_bb = false;
        g_do_cfg_remove_trampolin_bb = false;
        g_do_cfg_dom = false;
        g_do_cfg_pdom = false;
        g_do_loop_ana = false;
        g_do_cdg = false;
        g_do_cfg_remove_redundant_branch = false;
        g_do_cfg_invert_condition_and_remove_trampolin_bb = false;
        g_do_aa = false;
        g_do_md_du_analysis = false;
        g_is_support_dynamic_type = false;
        g_do_md_ssa = false;
        g_do_pr_ssa = false;
        g_compute_pr_du_chain = false;
        g_compute_nonpr_du_chain = false;        
        g_do_refine = false;
        g_do_refine_auto_insert_cvt = true;
        g_do_call_graph = false;
        g_do_ipa = false;
    }
    return true;
}
