(*********************************************************
 *                                                       *
 * this program reads a hypomac load file into pseudo-   *
 * memory and then prints the contents of that memory.   *
 *                                                       *
 * thus, the contents of the load file are sorted and    *
 * only 1 value (the most recent) is printed for each    *
 * location, regardless of how many times it was loaded. *
 *                                                       *
 * programmed by steve salisbury during fall 1980        *
 *                                                       *
 * system : pascal2 w/ purdue mace o.s. on cdc 6500/6600 *
 *                                                       *
 * Ported to UNIX January 1982 by Steve Salisbury        *
 *                                                       *
 * The name of the input file is the argument on the     *
 *    command line.  If there is no argument the name    *
 *    defaults to "loadfile".                            *
 *                                                       *
 * format:                                               *
 *                                                       *
 *  instruction:                                         *
 *   location  opcode         operand 1   operand 2      *
 * ______xxxx__yyyyy_______"zzzzzzzzzz"__1234567890      *
 *                                                       *
 *  data:                                                *
 *   location                      data                  *
 * ______xxxx________________1.000e+000                  *
 *                                                       *
 *  register                                             *
 *   register                  contents                  *
 * ________xx______________________yyyy                  *
 *********************************************************)

program loadprint( output, loadfile ) ;

const
   blank    = ' ';
   quote    = '"'; (* the quote character *)
   iheading = 'Location  Opcode         Operand 1   Operand 2';
   iheadng2 = '--------  ------      ------------  ----------';
   dheading = 'Location                      Data';
   dheadng2 = '--------              ------------';
   rheading = 'Register                  Contents';
   rheadng2 = '--------                ----------';
   isphdr   = 'Instruction Space:';
   dsphdr   = 'Data Space:';
   reghdr   = 'Registers:';
   badopmsg = 'Bad Op    ';
   badscmsg = 'Bad Sys Call ';

              (* hypomac opcodes go from 1 to 45 *)

   push        =   1;   pushr       =   2;   pushi       =   3;
   pushc       =   4;   pop         =   5;   popc        =   6;
   popr        =   7;   move        =   8;   swap        =   9;
   load        =  10;   loadr       =  11;   loada       =  12;
   loadi       =  13;   store       =  14;   storeregs   =  15;
   loadregs    =  16;   haltt       =  17;   add         =  18;
   sub         =  19;   negate      =  20;   mult        =  21;
   divide      =  22;   modulus     =  23;   orop        =  24;
   andop       =  25;   notop       =  26;   truncop     =  27;
   roundop     =  28;   floatop     =  29;   shift       =  30;
   gt          =  31;   ge          =  32;   lt          =  33;
   le          =  34;   eq          =  35;   ne          =  36;
   branch      =  37;   bctrue      =  38;   bcfalse     =  39;
   noop        =  40;   pak         =  41;   unpak       =  42;
   syscall     =  43;   call        =  44;   return      =  45;

   (* operand 1 of opcode 43 is one of the following *)

   open        =   1;   close       =   2;   getf        =   3;
   putf        =   4;   readline    =   5;   writeline   =   6;
   readbuff    =   7;   writebuff   =   8;   readint     =   9;
   readreal    =  10;   writeint    =  11;   writereal   =  12;
   clockit     =  13;   endofline   =  14;   endoffile   =  15;
   dumpit      =  16;   chrof       =  17;   ordof       =  18;
   sqrof       =  19;   dumpregs    =  20;   dumpinst    =  21;
   dumpdata    =  22;   linelimt    =  23;   instlimit   =  24;

   minispace =    0;    (* limits of ispace and dspace *)
   maxispace = 1023;
   mindspace =    1;
   maxdspace = 2047;

   minreg =  0;
   maxreg = 10;

type
   real    = shortreal;                  (* makes GPC real 32 bits 
                                            instead of default 64 bits --
                                            makes pascal real and c float
                                            compatible *)
   typs    = ( int, re, ch, alf, bool ) ;
   spaces  = ( ispace, dspace, regs ) ;

   alfa    = packed array [1..10] of char;

   oprange  = push .. return;            (* range of opcodes      *)
   sysrange = open  .. instlimit;        (* range of system calls *)

   varval = record
      case typ : typs of
        int  : ( intval  : integer ) ;
        re   : ( realval : real    ) ;
        ch   : ( charval : char    ) ;
        alf  : ( alfaval : alfa    ) ;
        bool : ( boolval : boolean )
      end; (* case, record *)

   dval = record
      val:varval;
      flg:boolean
      end;

   instruction = record   (* instruction format *)
      opcode : integer;
      oper1  : varval;
      oper2  : integer
      end; (* record *)

   ival = record
      val:instruction;
      flg:boolean
      end;

   register = record
      regnum : integer ;
      regval : integer
      end; (* record *)

   rval = record
      val : integer;
      flg : boolean   (* flag as to whether loaded or not *)
      end;

   loadrec = record  (* load file record format *)
      loc : integer ;
      case space : spaces of
        ispace : ( ivalue : instruction ) ;
        dspace : ( dvalue : varval      ) ;
        regs   : ( rvalue : register    )
      end; (* case, record *)

   filetype = file of loadrec;

   iaddress = minispace .. maxispace;
   daddress = mindspace .. maxdspace;
   regtype  = minreg .. maxreg;

var
   loadfile : filetype ;                 (* load file from pj compiler*)
   fname    : alfa;                      (* load file name as an arg. *)
   lrec     : loadrec ;                  (* a single record of loadf  *)
   mnem     : array[ oprange  ] of alfa; (* mnemonics for instructions*)
   sysm     : array[ sysrange ] of alfa; (* mnemonics for system calls*)
   isp      : array[ iaddress ] of ival;
   dsp      : array[ daddress ] of dval;
   reg      : array[ regtype  ] of rval;
   i        : integer;                   (* loop index *)

begin (* loadprint *)

           (* instruction mnemonics *)

mnem[ push      ] := 'Push      '; mnem[ pushr     ] := 'Pushr     ';
mnem[ pushi     ] := 'Pushi     '; mnem[ pushc     ] := 'Pushc     ';
mnem[ pop       ] := 'Pop       '; mnem[ popc      ] := 'Popc      ';
mnem[ popr      ] := 'Popr      '; mnem[ move      ] := 'Move      ';
mnem[ swap      ] := 'Swap      '; mnem[ load      ] := 'Load      ';
mnem[ loadr     ] := 'Loadr     '; mnem[ loada     ] := 'Loada     ';
mnem[ loadi     ] := 'Loadi     '; mnem[ store     ] := 'Store     ';
mnem[ storeregs ] := 'Storeregs '; mnem[ loadregs  ] := 'Loadregs  ';
mnem[ haltt     ] := 'Halt      '; mnem[ add       ] := 'Add       ';
mnem[ sub       ] := 'Sub       '; mnem[ negate    ] := 'Negate    ';
mnem[ mult      ] := 'Mult      '; mnem[ divide    ] := 'Divide    ';
mnem[ modulus   ] := 'Mod       '; mnem[ orop      ] := 'Or        ';
mnem[ andop     ] := 'And       '; mnem[ notop     ] := 'Not       ';
mnem[ truncop   ] := 'Trunc     '; mnem[ roundop   ] := 'Round     ';
mnem[ floatop   ] := 'Float     '; mnem[ shift     ] := 'Shift     ';
mnem[ gt        ] := 'Gt        '; mnem[ ge        ] := 'Ge        ';
mnem[ lt        ] := 'Lt        '; mnem[ le        ] := 'Le        ';
mnem[ eq        ] := 'Eq        '; mnem[ ne        ] := 'Ne        ';
mnem[ branch    ] := 'Branch    '; mnem[ bctrue    ] := 'Bctrue    ';
mnem[ bcfalse   ] := 'Bcfalse   '; mnem[ noop      ] := 'Noop      ';
mnem[ pak       ] := 'Pack      '; mnem[ unpak     ] := 'Unpack    ';
mnem[ syscall   ] := 'Syscall   '; mnem[ call      ] := 'Call      ';
mnem[ return    ] := 'Return    ';

            (* system call mnemonics *)

sysm[ open      ] := '      Open'; sysm[ close     ] := '     Close';
sysm[ getf      ] := '       Get'; sysm[ putf      ] := '       Put';
sysm[ readline  ] := '    Readln'; sysm[ writeline ] := '   Writeln';
sysm[ readbuff  ] := '  Readbuff'; sysm[ writebuff ] := ' Writebuff';
sysm[ readint   ] := '   Readint'; sysm[ readreal  ] := '  Readreal';
sysm[ writeint  ] := '  Writeint'; sysm[ writereal ] := ' Writereal';
sysm[ clockit   ] := '     Clock'; sysm[ endofline ] := '      Eoln';
sysm[ endoffile ] := '       Eof'; sysm[ dumpit    ] := '    Dumpit';
sysm[ chrof     ] := '       Chr'; sysm[ ordof     ] := '       Ord';
sysm[ sqrof     ] := '       Sqr'; sysm[ dumpregs  ] := '     Dumpr';
sysm[ dumpinst  ] := '     Dumpi'; sysm[ dumpdata  ] := '     Dumpd';
sysm[ linelimt  ] := ' Linelimit'; sysm[ instlimit ] := '   Instlim';

      (* clear memory and load *)

      for i := minispace to maxispace do
         isp[ i ].flg := false;

      for i := mindspace to maxdspace do
         dsp[ i ].flg := false;

      for i := minreg to maxreg do
         reg[ i ].flg := false;

   reset( loadfile, 'loadfile' );

   while not eof(loadfile) do begin (* for all records lrec in loadfile *)

      read( loadfile, lrec ) ;

      if lrec.space = ispace then begin (* instruction *)
         isp[ lrec.loc ].flg := true;
         isp[ lrec.loc ].val := lrec.ivalue
         end
      else if lrec.space = dspace then begin (* data *)
         dsp[ lrec.loc ].flg := true;
         dsp[ lrec.loc ].val := lrec.dvalue
         end
      else if lrec.space = regs then begin      (* register load *)
         reg[ lrec.rvalue.regnum ].flg := true; (*   14 jan 82   *)
         reg[ lrec.rvalue.regnum ].val := lrec.rvalue.regval
         end
   (* else *ignore bad "spaces"*   *)          (* 14 jan 82 *)
      end;

   writeln(isphdr);
   writeln;
   writeln(iheading);
   writeln(iheadng2);

   for i := minispace to maxispace do
      if isp[ i ].flg then with isp[ i ].val do begin
         write(i:8, blank:2 );
         if opcode in [push .. return] then
            write( mnem[opcode], blank:2 )
         else
            write( badopmsg, opcode:4, blank:2 );
         if not ( opcode in [swap, haltt, modulus,
            orop, andop, notop, gt, ge, lt, le, eq, ne,
            noop, pak, unpak, return] ) then begin
               case oper1.typ of
                  int  : if (opcode = syscall) then begin
                            if (oper1.intval in [open..instlimit]) then
                               write(blank:2, sysm[ oper1.intval ])
                            else (* bad system call *)
                               write(badscmsg, oper1.intval:1 )
                            end
                         else (* integer is really an integer *)
                         write( oper1.intval :12 );
                  re   : write( oper1.realval:12 );
                  ch   : write( quote:10, oper1.charval, quote );
                  alf  : write( quote,    oper1.alfaval, quote );
                  bool : write( oper1.boolval:12 )
               end; (* case *)
            if opcode in
               [push, pushc, pop, popc, move, load, loadr, loada,
                loadi, store, storeregs, loadregs, shift, call] then
               write( oper2:12 )   (* 2 operands *)
            end;
         writeln
         end;

   writeln;
   writeln(dsphdr);
   writeln;
   writeln(dheading);
   writeln(dheadng2);

   for i := mindspace to maxdspace do
      if dsp[ i ].flg then with dsp[ i ].val do begin
         write(i:8, blank:14 );
         if typ in [int,re,ch,alf,bool] then
            case typ of
               int  : writeln( blank:2,  intval:10      );
               re   : writeln( blank:2,  realval:10:5   );
               ch   : writeln( quote:10, charval, quote );
               alf  : writeln( quote,    alfaval, quote );
               bool : writeln( blank:2,  boolval:10     )
               end (* case *)
         end;

   writeln;
   writeln(reghdr);
   writeln;
   writeln(rheading);
   writeln(rheadng2);

   for i := minreg to maxreg do
      if reg[ i ].flg then
         writeln( i:8, blank:16, reg[ i ].val:10 )

end. (* loadprint *)
