#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_push(&id_dest->val);
  //printf("push %x\n",id_src->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&id_dest->val);
  operand_write(id_dest,&id_dest->val);
  //printf("pop %x\n",id_dest->val);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  rtl_mv(&cpu.esp,&cpu.ebp);
  rtl_pop(&cpu.ebp);
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) 
  {
    rtl_lr(&t0,R_AX,2);
    if((int16_t)t0<0)
    {
      rtl_addi(&t1,&tzero,0xffff);
      rtl_sr(R_DX,2,&t1);
    }
    else
    {
      rtl_sr(R_DX,2,&tzero);
    }
  }
  else 
  {
    rtl_lr(&t0,R_EAX,4);
    if((int32_t)t0<0)
    {
      rtl_addi(&t1,&tzero,0xffffffff);
      rtl_sr(R_EDX,4,&t1);
    }
    else
    {
      rtl_sr(R_EDX,4,&tzero);
    }
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) 
  {
    rtl_lr(&t0,R_AL,1);
    t0 = (int16_t)(int8_t)t0;
    rtl_sr(R_AX,2,&t0);
    //rtl_sr(int r, int width, const rtlreg_t* src1)
  }
  else 
  {
    rtl_lr(&t0,R_AX,2);

    t0 = (int32_t)(int16_t)t0;

    rtl_sr(R_EAX,4,&t0);
  }
  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  //printf("t2:0x%08x\n",t2);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
