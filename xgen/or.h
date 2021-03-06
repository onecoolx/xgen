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

author: Su Zhenyu
@*/
#ifndef __OR_H__
#define __OR_H__

namespace xgen {

class OR;
class ORBB;

typedef xcom::C<OR*> ORCt; //OR container
typedef Vector<OR*> ORVec; //OR vector

#define OFST_STORE_BASE       0
#define OFST_STORE_OFST       1
#define OFST_STORE_VAL        2
#define OFST_LOAD_BASE        0
#define OFST_LOAD_OFST        1

//OR Descriptor
#define OTD_code(o)             ((o)->code)
#define OTD_name(o)             ((o)->name)
#define OTD_unit(o)             ((o)->unit)
#define OTD_equ_or_types(o)     ((o)->equ_or_types)
#define OTD_srd_group(o)        ((o)->sr_desc_group)
#define OTD_sche_info(o)        ((o)->sche_info)
#define OTD_is_fake(o)          ((o)->is_fake)
#define OTD_is_bus(o)           ((o)->is_bus)
#define OTD_is_nop(o)           ((o)->is_nop)
#define OTD_is_volatile(o)      ((o)->is_volatile)
#define OTD_is_side_effect(o)   ((o)->is_side_effect)
#define OTD_is_asm(o)           ((o)->is_asm)
#define OTD_is_predicated(o)    ((o)->is_predicated)
#define OTD_is_call(o)          ((o)->isCallStmt)
#define OTD_is_cond_br(o)       ((o)->isConditionalBr)
#define OTD_is_uncond_br(o)     ((o)->isUnconditionalBr)
#define OTD_is_indirect_br(o)   ((o)->isIndirectBr)
#define OTD_is_return(o)        ((o)->is_return)
#define OTD_is_unaligned(o)     ((o)->is_unaligned)
#define OTD_is_store(o)         ((o)->is_store)
#define OTD_is_load(o)          ((o)->is_ld)
#define OTD_is_eq(o)            ((o)->is_eq)
#define OTD_is_lt(o)            ((o)->is_lt)
#define OTD_is_gt(o)            ((o)->is_gt)
#define OTD_is_movi(o)          ((o)->is_movi)
#define OTD_is_addi(o)          ((o)->is_addi)
#define OTD_is_subi(o)          ((o)->is_subi)
class ORTypeDesc {
public:
    OR_TYPE code;
    CHAR const* name;

    //Hardware unit.
    UNIT unit;

    //The equivalent or-types that are same in utilities for
    //different hardware function-unit.
    EquORTypes * equ_or_types;

    //Description of source operands and result operands.
    SRDescGroup<> * sr_desc_group;

    ORScheInfo sche_info;

    UINT is_predicated:1; //True if OR has a preidcate register.
    UINT is_fake:1;
    UINT is_bus:1;
    UINT is_nop:1;
    UINT is_volatile:1;
    UINT is_side_effect:1;
    UINT is_asm:1;
    UINT isCallStmt:1;
    UINT isConditionalBr:1;
    UINT isUnconditionalBr:1;
    UINT isIndirectBr:1;
    UINT is_return:1;
    UINT is_unaligned:1;
    UINT is_store:1;
    UINT is_ld:1;
    UINT is_eq:1;
    UINT is_lt:1;
    UINT is_gt:1;
    UINT is_movi:1;
    UINT is_addi:1;
    UINT is_subi:1;
};


//Target Operation Representation
#define OR_code(o)              ((o)->code)
#define OR_ct(o)                ((o)->container)
#define OR_clust(o)             ((o)->clust)
#define OR_id(o)                ((o)->id)
#define OR_order(o)             ((o)->order)
#define OR_bb(o)                ((o)->bb)
#define OR_unit(o)              OTD_unit(tmGetORTypeDesc(OR_code(o)))
#define OR_code_name(o)         OTD_name(tmGetORTypeDesc(OR_code(o)))
#define OR_dbx(o)               ((o)->dbx)
#define OR_is_call(o)           OTD_is_call(tmGetORTypeDesc(OR_code(o)))
#define OR_is_uncond_br(o)      OTD_is_uncond_br(tmGetORTypeDesc(OR_code(o)))
#define OR_is_indirect_br(o)    OTD_is_indirect_br(tmGetORTypeDesc(OR_code(o)))
#define OR_is_cond_br(o)        OTD_is_cond_br(tmGetORTypeDesc(OR_code(o)))
#define OR_is_return(o)         OTD_is_return(tmGetORTypeDesc(OR_code(o)))
#define OR_is_br(o)             (OR_is_cond_br(o) || OR_is_uncond_br(o) || \
                                 OR_is_return(o) || OR_is_call(o))
#define OR_is_predicated(o)     OTD_is_predicated(tmGetORTypeDesc(OR_code(o)))
#define OR_is_load(o)           OTD_is_load(tmGetORTypeDesc(OR_code(o)))
#define OR_is_store(o)          OTD_is_store(tmGetORTypeDesc(OR_code(o)))
#define OR_is_signed(o)         ((o)->u1.s1.is_signed)
#define OR_is_spill(o)          ((o)->u1.s1.is_spill)
#define OR_is_reload(o)         ((o)->u1.s1.is_reload)
#define OR_is_terminate(o)      ((o)->u1.s1.is_terminate_control_flow)
#define OR_is_need_compute_var_ofst(o)  ((o)->u1.s1.need_to_compute_var_ofst)
#define OR_is_mem(o)            (OR_is_store(o) || OR_is_load(o))
#define OR_is_fake(o)           OTD_is_fake(tmGetORTypeDesc(OR_code(o)))
#define OR_is_bus(o)            OTD_is_bus(tmGetORTypeDesc(OR_code(o)))
#define OR_is_nop(o)            OTD_is_nop(tmGetORTypeDesc(OR_code(o)))
#define OR_is_volatile(o)       OTD_is_volatile(tmGetORTypeDesc(OR_code(o)))
#define OR_is_side_effect(o)    OTD_is_side_effect(tmGetORTypeDesc(OR_code(o)))
#define OR_is_asm(o)            OTD_is_asm(tmGetORTypeDesc(OR_code(o)))
#define OR_is_unaligned(o)      OTD_is_unaligned(tmGetORTypeDesc(OR_code(o)))
#define OR_is_eq(o)             OTD_is_eq(tmGetORTypeDesc(OR_code(o)))
#define OR_is_lt(o)             OTD_is_lt(tmGetORTypeDesc(OR_code(o)))
#define OR_is_gt(o)             OTD_is_gt(tmGetORTypeDesc(OR_code(o)))
#define OR_is_movi(o)           OTD_is_movi(tmGetORTypeDesc(OR_code(o)))
#define OR_is_addi(o)           OTD_is_addi(tmGetORTypeDesc(OR_code(o)))
#define OR_is_subi(o)           OTD_is_subi(tmGetORTypeDesc(OR_code(o)))
#define OR_flag(o)              ((o)->u1.s1byte)
class OR {
protected:
    Vector<SR*, 2> m_opnd; //operand of micro operation
    Vector<SR*, 2> m_result; //result of micro operation
public:
    //each op has its own unique id.
    //DO NOT MODIFY 'id' DURING cleaning, cloning or copying of OR.
    UINT id;

    OR_TYPE code; //operation type, various to different target machine.

    //Container of OR, used for List operations,
    //only available when OR in list.
    ORCt * container;

    CLUST clust; //cluster, various to different target machine.
    INT order; //identify the ordinal of OR in BB.
    ORBB * bb;
    union {
        struct {
            BYTE is_signed:1; //Is OR signed?
            BYTE is_spill:1; //Is OR spilling operation?
            BYTE is_reload:1; //Is OR reloading operation?
            BYTE is_terminate_control_flow:1; //Is OR terminate control flow?

            //Set to true if OR has a dummy offset which indicated by VAR.
            //And the offset should be caclulated to be an integer
            //before emiting assembly.
            BYTE need_to_compute_var_ofst:1;
        } s1;
        BYTE s1byte;
    } u1;
    Dbx dbx;

public:
    OR() { init(); }
    virtual ~OR() { destroy(); }
    void init()
    {
        code = OR_UNDEF;
        container = NULL;
        clust = CLUST_UNDEF;
        id = 0;
        order = -1;
        bb = NULL;
        OR_flag(this) = 0;
        m_opnd.init();
        m_result.init();
        dbx.clean();
    }

    void destroy()
    {
        m_opnd.destroy();
        m_result.destroy();
    }

    void clean();
    virtual void clone(OR const* o);
    void copyDbx(IR const* ir);

    virtual CHAR const* dump(StrBuf & buf, CG * cg) const;
    virtual void dump(CG * cg) const;

    SR * get_opnd(INT i) const
    {
        ASSERT0(i <= m_opnd.get_last_idx() && i >= 0);
        return m_opnd.get(i);
    }

    SR * get_result(INT i) const
    {
        ASSERT0(i <= m_result.get_last_idx() && i >= 0);
        return m_result.get(i);
    }

    virtual SR * get_pred() //get predicate register
    {
        ASSERTN_DUMMYUSE(HAS_PREDICATE_REGISTER, ("target not support"));
        return get_opnd(0);
    }

    virtual SR * get_mov_to() { return get_result(0); }

    virtual SR * get_mov_from()
    { return get_opnd(HAS_PREDICATE_REGISTER + 0); }

    virtual SR * get_copy_to() { return get_result(0); }
    virtual SR * get_lda_result() { return get_result(0); }

    virtual SR * get_copy_from()
    { return get_opnd(HAS_PREDICATE_REGISTER + 0); }

    virtual SR * get_lda_base()
    { return get_opnd(HAS_PREDICATE_REGISTER + 0); }

    virtual LabelInfo const* getLabel() const
    {
        SR * sr = get_opnd(HAS_PREDICATE_REGISTER + 0);
        ASSERT0(sr && SR_is_label(sr));
        return SR_label(sr);
    }

    //OR format is : <- base, ofst, store_value0, store_value1, ...
    SR * get_store_ofst()
    {
        ASSERT0(OR_is_store(this));
        return get_opnd(HAS_PREDICATE_REGISTER + OFST_STORE_OFST);
    }

    //Return the base SR for memory load/store.
    SR * get_mem_base()
    {
        ASSERT0(OR_is_store(this) || OR_is_load(this));
        if (OR_is_store(this)) {
            return get_store_base();
        }
        return get_load_base();
    }

    //Get LOAD VALUE, the default LOAD has one load-value opnd.
    //There are may be multiple load-value opnds if target
    //is, for example SIMD machine.
    //Layout of opnds:
    //---------------------------------------------------------
    // 0         | 1    | 2      | 3         | 4         |     |
    // predicate | base | offset | load-val1 | load-val2 | ... |
    // register  |      |        |           |           |     |
    //---------------------------------------------------------
    //'idx': index of load-values.
    SR * get_load_val(UINT idx)
    {
        ASSERT0(OR_is_load(this));
        return get_result(idx);
    }

    UINT get_num_load_val() const { return result_num(); }

    //Get BASE of LOAD MEMORY ADDRESS.
    SR * get_load_base()
    {
        ASSERT0(OR_is_load(this));
        return get_opnd(HAS_PREDICATE_REGISTER + OFST_LOAD_BASE);
    }

    //Get OFFSET of BASE of LOAD MEMORY ADDRESS.
    SR * get_load_ofst()
    {
        ASSERT0(OR_is_load(this));
        return get_opnd(HAS_PREDICATE_REGISTER + OFST_LOAD_OFST);
    }

    //Get opnd SR, the default STORE has at least one store-value.
    //OR format is : <- base, ofst, store_value0, store_value1, ...
    //'idx': index of store-value.
    SR * get_store_val(UINT idx = 0)
    {
        ASSERT0(OR_is_store(this));
        return get_opnd(HAS_PREDICATE_REGISTER + OFST_STORE_VAL + idx);
    }

    UINT get_num_store_val() const
    { return opnd_num() - OFST_STORE_VAL - HAS_PREDICATE_REGISTER; }

    //OR format is : <- base, ofst, store_value0, store_value1, ...
    SR * get_store_base()
    {
        ASSERT0(OR_is_store(this));
        return get_opnd(HAS_PREDICATE_REGISTER + OFST_STORE_BASE);
    }
    INT get_opnd_idx(SR * sr) const;
    INT get_result_idx(SR * sr) const;
    virtual SR * get_imm_sr();

    UINT opnd_num() const { return tmGetOpndNum(OR_code(this)); }
    UINT result_num() const { return tmGetResultNum(OR_code(this)); }

    bool hasSideEffect() const { return OR_is_side_effect(this); }

    virtual bool is_equal(OR const* o) const;
    //Return true if 'o' depicted a label.
    bool is_label_or()
    {
        #ifdef _DEBUG_
        if (OR_code(this) == OR_label) {
            ASSERT0(getLabel());
        }
        #endif
        return OR_code(this) == OR_label;
    }
    bool isConditionalBr() { return OR_is_cond_br(this);    }
    bool isUnconditionalBr() const { return OR_is_uncond_br(this); }
    bool isIndirectBr() const { return OR_is_indirect_br(this); }
    bool is_return() const { return OR_is_return(this); }
    bool is_call() const { return OR_is_call(this); }
    bool isMultiConditionalBr() const { return false; }
    bool isNeedComputeVAROfst() const
    { return OR_is_need_compute_var_ofst(this); }

    //Return true if ir terminates the control flow.
    bool is_terminate() const { return OR_is_terminate(this); }
    bool isSpadjust() const { return OR_code(this) == OR_spadjust; }

    //Set STORE VALUE, it should be a register, the default STORE
    //has one store-value opnd.
    //There are may be multiple store-value opnds if target is SIMD machine.
    //Layout of opnds:
    //-----------------------------------------------------------
    // 0         | 1    | 2      | 3          | 4          |     |
    // predicate | base | offset | store-val1 | store-val2 | ... |
    // register  |      |        |            |            |     |
    //-----------------------------------------------------------
    //'idx': index of store-values.
    void set_store_val(SR * val, UINT idx = 0)
    {
        ASSERT0(OR_is_store(this));
        set_opnd(HAS_PREDICATE_REGISTER + OFST_STORE_VAL + idx, val);
    }

    //Set BASE of STORE MEMORY ADDRESS.
    void set_store_base(SR * base)
    {
        ASSERT0(OR_is_store(this));
        set_opnd(HAS_PREDICATE_REGISTER + OFST_STORE_BASE, base);
    }

    //Set OFFSET of BASE of STORE MEMORY ADDRESS.
    void set_store_ofst(SR * ofst)
    {
        ASSERT0(OR_is_store(this));
        set_opnd(HAS_PREDICATE_REGISTER + OFST_STORE_OFST, ofst);
    }

    //Set result register, default load-operation has one result.
    void set_load_val(SR * val, UINT idx = 0)
    {
        ASSERT0(OR_is_load(this));
        set_result(idx, val);
    }

    //OR format is : result <- base, ofst
    void set_load_base(SR * base)
    {
        ASSERT0(OR_is_load(this));
        set_opnd(HAS_PREDICATE_REGISTER + OFST_LOAD_BASE, base);
    }

    //OR format is : result <- base, ofst
    void set_load_ofst(SR * ofst)
    {
        ASSERT0(OR_is_load(this));
        set_opnd(HAS_PREDICATE_REGISTER + OFST_LOAD_OFST, ofst);
    }

    void set_opnd(INT i, SR * sr)
    {
        ASSERT0(sr && (UINT)i < opnd_num());
        m_opnd.set(i, sr);
    }

    void set_result(INT i, SR * sr)
    {
        ASSERT0(sr && (UINT)i < result_num());
        m_result.set(i, sr);
    }

    virtual void set_pred(SR * v) //set predicate register
    {
        ASSERTN_DUMMYUSE(HAS_PREDICATE_REGISTER, ("target not support"));
        set_opnd(0, v);
    }

    virtual void set_mov_from(SR * v)
    { set_opnd(HAS_PREDICATE_REGISTER + 0, v); }

    virtual void set_copy_from(SR * v)
    { set_opnd(HAS_PREDICATE_REGISTER + 0, v); }

    virtual void set_lda_base(SR * v)
    { set_opnd(HAS_PREDICATE_REGISTER + 0, v); }

    virtual void setLabel(SR * v)
    { set_opnd(HAS_PREDICATE_REGISTER + 0, v);}

    virtual void set_lda_result(SR * v) { set_result(0, v); }
    virtual void set_mov_to(SR * v) { set_result(0, v); }
    virtual void set_copy_to(SR * v) { set_result(0, v); }
};


class ORList : public List<OR*> {
public:
    ORList() {}
    virtual ~ORList() {}
    void copyDbx(IR const* ir)
    {
        ASSERT0(ir);
        if (IR_ai(ir) == NULL) { return; }
        DbxAttachInfo * da = (DbxAttachInfo*)IR_ai(ir)->get(AI_DBX);
        if (da == NULL) { return; }
        for (OR * o = get_head(); o != NULL; o = get_next()) {
            OR_dbx(o).copy(da->dbx);
        }
    }

    void copyDbx(Dbx const* dbx)
    {
        ASSERT0(dbx);
        for (OR * o = get_head(); o != NULL; o = get_next()) {
            OR_dbx(o).copy(*dbx);
        }
    }

    void set_pred(IN SR * pred)
    {
        ASSERT0(pred);
        for (OR * o = get_head(); o != NULL; o = get_next()) {
            o->set_pred(pred);
        }
    }
    virtual void append_tail(OR * o);
    virtual void append_tail(ORList & ors);
    void dump(CG * cg);
};


typedef xcom::Hash<OR*> OR_HASH; //OR hash table


//
//START ORMgr
//
class ORMgr : public Vector<OR*> {
protected:
    List<OR*> m_free_or_list;
    CG * m_cg;
    SRMgr * m_sr_mgr;
public:
    ORMgr(SRMgr * srmgr);
    virtual ~ORMgr() { clean(); }
    void clean();
    virtual OR * allocOR();

    OR * getOR(UINT id);
    virtual OR * genOR(OR_TYPE ort, CG * cg);
    virtual void freeOR(IN OR * o);
    virtual void freeORList(IN ORList & o);
};
//END ORMgr

} //namespace xgen
#endif
