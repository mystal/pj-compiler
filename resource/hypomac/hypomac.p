(**********************************************************************)
(*                                                                    *)
(*   program    : hypomac (hypothetical machine) for cs 502           *)
(*                                                                    *)
(*   programmer : frances rosen, purdue university                    *)
(*                                                                    *)
(*   date       : november 21, 1979, mod. feb. 19, 1980               *)
(*                                                                    *)
(*   system     : pascal on cdc 6000, purdue mace o.s.                *)
(*                                                                    *)
(*   input      : a file of records containing instructions, data     *)
(*                and register initializations.                       *)
(*                                                                    *)
(*   output     : if an error is found, output is a machine dump.     *)
(*                otherwise, the user must include i/o in his         *)
(*                machine instructions.  the six temporary files are  *)
(*                renamed by the user and used for his i/o.           *)
(*                                                                    *)
(*   modifications for VAX 11/780 :                                   *)
(*                                                                    *)
(*          1) change all underscroes "_" to quotes "'"               *)
(*                                                                    *)
(*          2) include type definition for alfa                       *)
(*                                                                    *)
(*          3) redefined numbset to have 10 entries (0..9) instead    *)
(*             of CDC's 58 max entries (a..<) which, by the way are   *)
(*             in VAX ascending order                                 *)
(*                                                                    *)
(*          4) changed the values of limit : the maximun value for    *)
(*                                           an unsigned integer      *)
(*                                   lim1  : max exponent value       *)   
(*                                   lim2  : min exponent value       *)  
(*                                                                    *)
(*          5) changed function tena to reflect limited powers of 2   *) 
(*                                                                    *)
(*          6) Wrote procedure "rename" in VAX/VMS dependent Pascal.  *)
(*             It assigns as file descriptor to a file name given as  *) 
(*             a string (rename is no longer extern).                 *)
(*                                                                    *)
(*             This required the change of all references to the      *)
(*             constant "open" to "opin"                              *)
(*                                                                    *)
(*          7) changed maxordch to 127 (octal 177) to reflect ascii   *)
(*             character set instead of 63, the CDC character set     *)
(*                                                                    *)
(*          8) changed shift operation to mod 32 rather than 60       *)
(*                                                                    *)
(*          9) Removed all references to procedure rename             *)
(*                                                                    *)
(*         10) replaced function call to clock with the int value 0   *)
(*                                                                    *)
(*         11) re-typed "real" declaration to be "shortreal"          *)
(*             For the GPC compiler "real" was 8 bytes (64 bits) long *)
(*             This caudes an alignment with 4-byte float (32 bits    *)
(*             in the C-DEF for the loadfile                          *) 
(*                                                                    *)
(*   hypomac simulates a simple stack-oriented machine and simple     *)
(*   operating system.  it is used to run code produced in cs 502,    *)
(*   compiler construction (taught by d. comer).  hypomac allows      *)
(*   the user to read and write integers, reals, and characters, and  *)
(*   supports dynamic opening of files for input and output.  both    *)
(*   direct and indirect addressing are allowed.  hypomac is a        *)
(*   strongly typed machine, and type-checking is done at runtime.    *)
(*   the instruction space is completely separated from the data      *)
(*   space and no instructions exist which can modify the instructions*)
(*   after they have been loaded.  see f. rosen or d. comer for a     *)
(*   copy of the hypomac document, or for more information.           *)
(*                                                                    *)
(**********************************************************************)

program machine(loadfile,output,t1,t2,t3,t4,t5,t6);
   const
      initmes = '  hypomac, version 2.2 of april 24, 1980';
      minus = '-';
      blank = ' ';
      dpreg = 1;   (* display pointer *)
      spreg = 2;   (* stack pointer *)

      (*  opcodes *)

      badopc   =   0;
      push     =   1;
      pushr    =   2;
      pushi    =   3;
      pushc    =   4;
      pop      =   5;
      popc     =   6;
      popr     =   7;
      move     =   8;
      swap     =   9;
      load     =  10;
      loadr    =  11;
      loada    =  12;
      loadi    =  13;
      store    =  14;
      stregs   =  15;
      ldregs   =  16;
      haltt    =  17;
      add      =  18;
      sub      =  19;
      negate   =  20;
      mult     =  21;
      divide   =  22;
      modulus  =  23;
      orop     =  24;
      andop    =  25;
      notop    =  26;
      truncop  =  27;
      roundop  =  28;
      float    =  29;
      shift    =  30;
      gt       =  31;
      ge       =  32;
      lt       =  33;
      le       =  34;
      eq       =  35;
      ne       =  36;
      b        =  37;
      bct      =  38;
      bcf      =  39;
      noop     =  40;
      pak      =  41;
      unpak    =  42;
      syscall  =  43;
      call     =  44;
      return   =  45;

      (* system calls *)

      opin =      1; (* jda *)
      close    =  2;
      getf     =  3;
      putf     =  4;
      readlin  =  5;
      writlin  =  6;
      readbuf  =  7;
      writbuf  =  8;
      readi    =  9;
      readr    = 10;
      writint  = 11;
      writreal = 12;
      clockit  = 13;
      endoflin = 14;
      endoffil = 15;
      dumpit   = 16;
      chrof    = 17;
      ordof    = 18;
      sqrof    = 19;
      dumpr    = 20;
      dumpi    = 21;
      dumpd    = 22;
      linelim  = 23;
      instlim  = 24;
      minfnct  =  1;
      maxfnct  = 24;
      minispace=   0;
      maxispace=2048;
      mindspace=   1;
      maxdspac=4096;
      minregs  =   0;
      maxregs  =   9;
      minopcode =  1;
      maxopcode = 45;
      minfiles =   1;
      maxfiles =   6;
      minordch =   0;
      maxordch = 128;
      icachesize= 10;

      stackerr = '0error - stack reference out of range   ';
      regserr  = '0error - register reference out of range';
      dspacerr = '0error - dspace reference out of range  ';
      ispacerr = '0error - ispace reference out of range  ';
      afilerr  = '0error - no more inactive files         ';
      filerr   = '0error - too many files active          ';
      dhead    = '0          data area dump               ';
      shead    = '0          stack dump                   ';
      insttop  = '0          instruction space dump';
      regtop   = '0          register dump';
      cachetop = '0          last 10 instructions';
      machtop  = '0          machine dump';
      disptop  = '0          display dumps';
      styperr  = '    stack[';
      dtyperr  = '   dspace[';
      errdivide = '0value error - attempted division by zero';
      erreof    = '0file error - unexpected end of file';
      ilimerr1  = '0*** instruction limit exceeded ';
      ilimerr2  = '   executed, max was ';
      errlim1   = '0*** linelimit exceeded on file ';
      errlim2   = '   number of lines printed : ';
      errlim3   = '   maximum number allowed : ';
      warnfile  = '0file warning - file ';
      errfile1  = '0file error - file ';
      errfilop  = ' already opened';
      errfilno  = ' not present';
      errchr    = '0value error - bad chr value';
      errsqr    = '0type error - bad type for sqr';
      errbadop  = '0value error - opcode = 0';
      erropund  = '0type error - cannot push undefined value';
      errlong   = '0value error - real constant too long';
      errbig    = '0value error - real constant too large';
      errnumb   = '0type error - number expected';
      intoverflow =
          '0value warning - integer too big, truncated to maxint';
      warnint =
          '0value warning - probable integer overflow, set to 0 ';
      dpdumperr =
       '0dump error - display pointer out of range, cannot dump display';
      dptyperr  =
       '0dump error - back pointer not integer, cannot continue dumping';
      erreltyp  = '0type error - undefined element in relaton';
      errprog   = '0machine error - (cpu abort)';
      erroutput = '0file error - output is a reserved file name';
      errfunct  = '0error - system function call out of range';
      erradd1   = '0address error - indirect bit set but dspac[';
      erradd2   = '] is not an integer';
      erraddneg = '0address error - cannot calculate negative address';
      errundef  = '0arith. error - undefined result, 0.0 substituted';
      warnpush  = '0warning - 0 or negative count for push';
      errfinst  = '0 error in first instruction';
      errsp     = '0 sp out of range - all defined values dumped';

   type

      real = shortreal;                   (* jda *)

      alfa = packed array[1..10] of char; (* jda *)

      line = array[1..80] of char;
      filenames = record
                     fname : alfa;         (* external file name *)
                     aname : alfa;         (* internal file name *)
                     linelimt  : integer;  (* file line limit *)
                     linenum : integer     (* number of lines printed *)
                     end;
      message = packed array[1..40] of char;
      spaces = (ispce,dspce,rgs);
      typs = (int,re,ch,alf,bool,und);

      register = record
                    regnum : integer;
                    regval : integer
                    end;

      datawords = record
                     case typ : typs of
                        int : (intval : integer);
                        re  : (realval : real);
                        ch  : (charval : char);
                        alf : (alfval  : alfa);
                        bool: (boolval : boolean);
                        und : (undval  : alfa)
                        end;

      instwords = record
                     opcode : integer;
                     oper1  : datawords;
                     oper2  : integer
                     end;
   var
      numbset : set of '0'..'9'; (* jda *)
      icache : array[0..9] of instwords;  (* holds 10 most recently *)
      icacheptr : 0..9;                   (* executed instructions  *)
      fnames : array[0..maxfiles] of filenames; (* names in use    *)
      anames : array[1..maxfiles] of filenames; (* available names *)
      fnamptr : integer;
      anamptr : integer;
      regs : array[minregs..maxregs] of integer; (* registers *)
      dspac : array[0..maxdspac] of datawords;   (* data space *)
      ispace : array[0..maxispace] of instwords; (* instruction space *)
      minstack : integer;
      maxinst  : integer;
      t1,t2,t3,t4,t5,t6 : text;
      loadfile : file of record
                  loc : integer;
                  case spce : spaces of
                     ispce : (ivalue : instwords);
                     dspce : (dvalue : datawords);
                     rgs   : (rvalue : register)
                     end;
      done : boolean;
      curinst : instwords;
      iptr  : integer;
      indadds,              (* number of indirect addresses *)
      diradds,              (* number of direct addresses *)
      instnumb : integer;   (* number of instructions executed*)
      opcodenames : array[0..maxopcode] of alfa;
      outname : alfa;

      (* forward declarations of procedures and functions *)

procedure addline(locn : integer);                       forward;
function  convrtr(var string : line; len : integer):real;forward;
procedure dobuf(var c : char; var f : text);             forward;
procedure dodumpdisp;                                    forward;
procedure dodumps(dumptype : integer);                   forward;
procedure doget(var f:text);                             forward;
procedure domath(curop : integer);                       forward;
procedure docall(op1,op2 : integer);                     forward;
procedure doint(var f:text; var i :integer);             forward;
procedure doreadln(var f:text);                          forward;
procedure doreal(var f:text; var r:real);                forward;
procedure dorels(relat:integer);                         forward;
procedure doret;                                         forward;
procedure dosyscall(fnct:integer);                       forward;
procedure dumpdspace(bot,top : integer; hding : message);forward;
procedure dumpdword(x : datawords);                      forward;
procedure dumpispace(bot,top : integer);                 forward;
procedure dumpiword(i : instwords);                      forward;
procedure dumpregs(bot,top : integer);                   forward;
function  effaddr(op : integer) : integer;               forward;
procedure execute;                                       forward;
procedure fetch;                                         forward;
procedure fixflename(var talf : alfa);                   forward;
function  fsearch(fnm:alfa):integer;                     forward;
function  getop1(op1 : datawords): integer;              forward;
procedure initialize;                                    forward;
function  inrange(element,bot,top : integer;
                 msg : message): boolean;                forward;
procedure intwrite(i : integer);                         forward;
function  istyp(element:typs; typex:typs; msg:alfa;
               enum : integer): boolean;                 forward;
procedure progstop;                                      forward;
procedure readfile;                                      forward;
procedure readint(var f : text; var ival : integer);     forward;
procedure readreal(var f : text; var rval : real);       forward;
function  tena(e : integer) : real;                      forward;
procedure terminate;                                     forward;

procedure haltexecution;     (* JDA *)
var
  x,y:integer;
  z : real;
begin

   writeln;
   writeln('THIS IS A NORMAL TERMINATION... IT WILL BE FORCED THROUGH A DIVIDE BY 0');
   writeln;
   x:=5;
   y:=x-5;
   z:=x/y  (* divide by zero *)
end;

(**********)
(*   addline - make sure a line can be added to a file *)
(**********)

procedure addline (* locn : integer *);
   begin
      with fnames[locn] do
         if linenum > linelimt then begin
            write(errlim1,fname,errlim2);
            intwrite(linenum);
            write(errlim3);
            intwrite(linelimt);
            writeln;
            progstop
            end
         else
            linenum := linenum + 1
      end; (* addline *)



(**********)
(*   convrtr - convert string to real number                 *)
(*   ref : jensen & wirth, pascal user's manual              *)
(**********)

function convrtr(* (var string : line; len : integer) : real*);
   const
      limit =  maxint;            (* max unsigned integer jda - 2147483647 for VP *)
      lim1  =  38;                (* max exponent  jda *)
      lim2  = -38;                (* min exponent  jda *)
   var
      y     :  real;
      a,i,e :  integer;
   begin
      string[len+1] := blank;
      len := 1;
      a := 0;
      e := 0;
      repeat
         if a < limit then
            a := 10 * a + ord(string[len]) - ord('0')
         else
            e := e + 1;
         len := len + 1
      until not (string[len] in numbset);
      if string[len] = '.' then begin   (* get fraction *)
         len := len + 1;
         while string[len] in numbset do begin
            if a < limit then begin
               a := 10 * a + ord(string[len]) - ord('0');
               e := e - 1
               end;
            len := len + 1
            end
         end;
      if e < lim2 then begin
         a := 0;
         e := 0
         end
      else if e > lim1 then begin
         writeln(errbig);
         progstop
         end;
      if a > maxint then (* jda *)
         y := ((a+1) div 2) * 2.0
      else
         y := a;
      if e < 0 then
         convrtr := y/tena(-e)
      else if e <> 0 then
         convrtr := y*tena(e)
      else
         convrtr := y
      end; (* convrtr *)



(**********)
(*   dumpdword : dump the contents of a location in dspace *)
(**********)

procedure dumpdword(* x : datawords *);
   const
      typreal = ' type is real ';
      typalf  = ' type is alfa ';
      typchar = ' type is char ';
      typbool = ' type is boolean ';
      typund  = ' type is undefined ';
      typint  = ' type is integer ';
      valueof = 'value is ';
   begin
      case x.typ of
         int  : begin
                   write(typint:20,valueof);
                   intwrite(x.intval);
                   writeln
                   end;
         re  : writeln(typreal:20,valueof,x.realval:15);
         alf : writeln(typalf:20,valueof,x.alfval:15);
         ch  : writeln(typchar:20,valueof,x.charval:15);
         bool: writeln(typbool:20,valueof,x.boolval:15);
         und : writeln(typund:20,valueof,x.undval:15)
         end (* case *)
      end; (* dumpdword *)



(**********)
(*   dumpdspace - dump dspace from bot to top *)
(**********)

procedure dumpdspace(* bot,top : integer; hding : message *);
   var
      i : integer;
   begin
      if not inrange(bot,mindspace,maxdspac,dspacerr) then
         progstop;
      if not inrange(top,mindspace,maxdspac,dspacerr) then
         progstop;
      writeln(hding);
      for i := bot to top do
         if dspac[i].typ <> und then begin
            write(' dspac[',i:1,'] : ');
            dumpdword(dspac[i])
            end
      end; (* dumpdspace *)



(**********)
(*   dumpispace - dump ispace from bot to top *)
(**********)

procedure dumpispace(* bot,top : integer *);
   var
      i : integer;
   begin
      if not inrange(bot,minispace,maxispace,ispacerr) then
         progstop;
      if not inrange(top,minispace,maxispace,ispacerr) then
         progstop;
      writeln(insttop);
      for i := bot to top do
         if ispace[i].opcode <> badopc then begin
            write(' ispace[',i:1,'] : ');
            dumpiword(ispace[i])
            end
      end; (* dumpispace *)



(**********)
(*   dumpiword - dump the value of an ispace location *)
(*********)

procedure dumpiword(* i : instwords *);
   const
      opcodeeq = ' opcode = ';
      oper1int = ' oper1 has type integer and value ';
      oper1re  = ' oper1 has type real and value ';
      oper1ch  = ' oper1 has type char and value ';
      oper1bl  = ' oper1 has type boolean and value ';
      oper1alf = ' oper1 has type alfa and value ';
      oper1und = ' oper1 is undefined ';
      oper2eq  = ' oper2 = ';
   begin
      with i do begin
         write(opcodeeq); write(opcodenames[opcode]);
         case oper1.typ of
            int : begin
                     write(oper1int);
                     intwrite(oper1.intval)
                     end;
            re  : write(oper1re,oper1.realval:13);
            ch  : write(oper1ch,oper1.charval:13);
            alf : write(oper1alf,oper1.alfval:13);
            bool: write(oper1bl,oper1.boolval:13);
            und : write(oper1und)
            end; (* case *)
         writeln(oper2eq,oper2)
         end
      end; (* dumpiword *)



(**********)
(*   dumpregs - dump the registers from bot to top *)
(**********)

procedure dumpregs(* bot,top : integer *);
   const
      eq = ' = ';
   var
      i : integer;
   begin
      if not inrange(bot,minregs,maxregs,regserr) then
         progstop;
      if not inrange(top,minregs,maxregs,regserr) then
         progstop;
      writeln(regtop);
      for i := bot to top do begin
         write(' register ',i:1,eq);
         intwrite(regs[i]);
         writeln
         end
      end; (* dumpregs *)



(**********)
(*   dodumpdisp - dump (up to) 10 displays *)
(**********)

procedure dodumpdisp;
   const
      maxdispdump = 10;
      disprange   =  5;
   var
      ctr : 0..maxdispdump;
      dp : integer;

   procedure dumpdisplay(dp:integer);
      var
         i : integer;
         bot : integer;
         top : integer;
      begin
         if dp - disprange < mindspace then
            bot := mindspace
         else
            bot := dp - disprange;
         if dp + disprange > maxdspac then
            top := maxdspac
         else
            top := dp + disprange;
         for i := bot to top do begin
            write('   dspac[',i:1,'] : ');
            dumpdword(dspac[i])
            end
         end; (* dumpdisplay *)

   begin (* dodumpdisp *)
      ctr := 0;
      dp := regs[dpreg];
      writeln(disptop);
      repeat (* do for each display to be dumped *)
         writeln(minus);
         ctr := ctr + 1;
         if (dp < mindspace) or (dp > maxdspac) then begin
            writeln(dpdumperr);
	    haltexecution
            end;
         dumpdisplay(dp);
         if not istyp(dspac[dp].typ,int,styperr,dp) then begin
            writeln(dptyperr);
	    haltexecution
            end;
         dp := dspac[dp].intval
      until (dp = 0) or (ctr = maxdispdump)
      end; (* dodumpdisp *)



(**********)
(*   dodumps - execute one of the dump system function *)
(**********)

procedure dodumps(* dumptype : integer *);
   var
      sp : integer;
   begin
      sp := regs[spreg];
      if not inrange(sp,minstack,maxdspac,stackerr) then
         progstop;
      if not inrange(sp-1,minstack,maxdspac,stackerr) then
         progstop;
      if not istyp(dspac[sp].typ,int,styperr,sp) then
         progstop;
      if not istyp(dspac[sp-1].typ,int,styperr,sp-1) then
         progstop;
      case dumptype of
         dumpd : dumpdspace(dspac[sp].intval,dspac[sp-1].intval,dhead);
         dumpi : dumpispace(dspac[sp].intval,dspac[sp-1].intval);
         dumpr : dumpregs(dspac[sp].intval,dspac[sp-1].intval)
         end; (* case *)
      regs[spreg] := sp - 2
      end; (* dodumps *)



(**********)
(*   doget - execute the getf system function *)
(**********)

procedure doget(* var f : text *);
   begin
      if eof(f) then begin
         writeln(erreof);
         progstop
         end
      else
         get(f)
      end; (* doget *)



(**********)
(*   domath - execute one of add,sub,mult, or divide *)
(**********)

procedure domath(* curop : integer *);
   var
      sp : integer;
      op1 : integer;
      tempres : integer;
   begin
      op1 := getop1(curinst.oper1);
      sp := regs[spreg] - 1;
      if not inrange(sp,minstack,maxdspac,stackerr) then
         progstop;
      if not inrange(sp+1,minstack,maxdspac,stackerr) then
         progstop;
      if op1 = 0 then begin
         if not istyp(dspac[sp].typ,int,styperr,sp) then
            progstop;
         if not istyp(dspac[sp+1].typ,int,styperr,sp+1) then
            progstop;
         case curop of
            add     : dspac[sp].intval := dspac[sp].intval +
                         dspac[sp+1].intval;
            sub     : dspac[sp].intval := dspac[sp].intval -
                         dspac[sp+1].intval;
            mult    : begin
                         tempres := dspac[sp].intval *
                            dspac[sp+1].intval;
                         if (tempres = 0) and (dspac[sp].intval <> 0)
                            and (dspac[sp+1].intval <> 0) then
                               writeln(warnint);
                         dspac[sp].intval := tempres
                         end;
            divide  : begin
                         if dspac[sp+1].intval = 0 then begin
                            writeln(errdivide);
                            progstop
                            end;
                         if abs(dspac[sp+1].intval) > maxint then begin
                            writeln(intoverflow);
                            dspac[sp+1].intval := maxint
                            end;
                         if abs(dspac[sp].intval) > maxint then begin
                            writeln(intoverflow);
                            dspac[sp].intval := maxint
                            end;
                         dspac[sp].intval := dspac[sp].intval div
                            dspac[sp+1].intval
                         end
             end (* case *)
          end
      else begin
         if not istyp(dspac[sp].typ,re,styperr,sp) then
            progstop;
         if not istyp(dspac[sp+1].typ,re,styperr,sp+1) then
            progstop;
         case curop of
            add     : dspac[sp].realval := dspac[sp].realval +
                         dspac[sp+1].realval;
            sub     : dspac[sp].realval := dspac[sp].realval -
                          dspac[sp+1].realval;
            mult    : begin
                         dspac[sp].realval := dspac[sp].realval *
			    dspac[sp+1].realval
                         end;
            divide  : begin
                         if dspac[sp+1].realval = 0.0 then begin
                           writeln(errdivide);
                           progstop
                           end;
                         dspac[sp].realval := dspac[sp].realval /
			    dspac[sp+1].realval
                         end
           end (* case *)
         end;
      regs[spreg] := sp
      end; (* domath *)



(**********)
(*   dobuf - execute the readbuf system function *)
(**********)

procedure dobuf(* var c : char; var f : text *);
   begin
      if eof(f) then begin
         writeln(erreof);
         progstop
         end
      else
         c := f^
      end; (* dobuf *)



(**********)
(*   docall - execute the call instruction *)
(**********)

procedure docall(* op1,op2 : integer *);
   const
      doffset   = 13;
      regoffset = 10;
   var
      tsp : integer;
      sp : integer;
      olddp : integer;
      i : integer;
   begin
      sp := regs[spreg]+1;
      if not inrange(sp,minstack,maxdspac,stackerr) then
         progstop;
      if not inrange(sp+doffset+op1-1,minstack,maxdspac,stackerr) then
         progstop;
      olddp := regs[dpreg];
      if not inrange(olddp+1,mindspace,maxdspac,dspacerr) then
         progstop;
      if not inrange(olddp+doffset+op1-2,mindspace,maxdspac,stackerr)
         then progstop;
      tsp := sp;
      dspac[sp].typ := int;
      dspac[sp].intval := olddp;
      sp := sp + 1;
      dspac[sp].typ := int;
      dspac[sp].intval := 0;
      dspac[olddp+1].typ := int;
      dspac[olddp+1].intval := tsp;
      sp := sp + 1;
      dspac[sp].typ := int;
      dspac[sp].intval := iptr;
      sp := sp + regoffset;     (* set aside place for registers *)
      for i := 0 to op1 - 2 do begin  (* move data area pointers *)
         sp := sp + 1;
         dspac[sp] := dspac[olddp+doffset+i]
         end;
      sp := sp + 1;
      dspac[sp].typ := int;
      dspac[sp].intval := sp+1;   (* pointer to own data area *)
      regs[dpreg] := tsp;
      regs[spreg] := sp;
      iptr := op2
      end (* docall *);



(**********)
(*   doint - execute the readi system function *)
(**********)

procedure doint(* var f : text; var i : integer *);
   begin
      if eof(f) then begin
         writeln(erreof);
         progstop
         end
      else   (* read the integer *)
         readint(f,i)
      end; (* doint *)



(**********)
(*   doreadln - execute the system readlin function *)
(**********)

procedure doreadln(* var f : text *);
   begin
      if eof(f) then begin
         writeln(erreof);
         progstop
         end
      else
         readln(f)
      end; (* doreadln *)



(**********)
(*   doreal - execute the system readr function *)
(**********)

procedure doreal(* var f : text; var r : real *);
   begin
      if eof(f) then begin
         writeln(erreof);
         progstop
         end
      else   (* read the real number *)
         readreal(f,r)
      end; (* doreal*)



(**********)
(*   dorels - execute the gt, ge, lt, le, eq, ne instructions *)
(**********)

procedure dorels(* relat : integer *);
   var
      cc : boolean;
      sp : integer;
   begin
      sp := regs[spreg];
      if not inrange(sp,minstack,maxdspac,stackerr) then
         progstop;
      if not inrange(sp-1,minstack,maxdspac,stackerr) then
         progstop;
      cc := false;
      case dspac[sp].typ of

         int : if not istyp(dspac[sp-1].typ,int,styperr,sp-1) then
                  progstop
               else case relat of
                  gt : if dspac[sp-1].intval > dspac[sp].intval then
                          cc := true;
                  lt : if dspac[sp-1].intval < dspac[sp].intval then
                          cc := true;
                  ge : if dspac[sp-1].intval >= dspac[sp].intval then
                          cc := true;
                  le : if dspac[sp-1].intval <= dspac[sp].intval then
                          cc := true;
                  eq : if dspac[sp-1].intval = dspac[sp].intval then
                          cc := true;
                  ne : if dspac[sp-1].intval <> dspac[sp].intval then
                          cc := true
                  end; (* case *)

         re  : if not istyp(dspac[sp-1].typ,re,styperr,sp-1) then
                  progstop
               else case relat of
                  gt : if dspac[sp-1].realval > dspac[sp].realval then
                          cc := true;
                  lt : if dspac[sp-1].realval < dspac[sp].realval then
                          cc := true;
                  ge : if dspac[sp-1].realval >= dspac[sp].realval then
                          cc := true;
                  le : if dspac[sp-1].realval <= dspac[sp].realval then
                          cc := true;
                  eq : if dspac[sp-1].realval = dspac[sp].realval then
                          cc := true;
                  ne : if dspac[sp-1].realval <> dspac[sp].realval then
                          cc := true
                  end; (* case *)
         alf : if not istyp(dspac[sp-1].typ,alf,styperr,sp-1) then
                  progstop
               else case relat of
                  gt : if dspac[sp-1].alfval > dspac[sp].alfval then
                          cc := true;
                  lt : if dspac[sp-1].alfval < dspac[sp].alfval then
                          cc := true;
                  ge : if dspac[sp-1].alfval >= dspac[sp].alfval then
                          cc := true;
                  le : if dspac[sp-1].alfval <= dspac[sp].alfval then
                          cc := true;
                  eq : if dspac[sp-1].alfval = dspac[sp].alfval then
                          cc := true;
                  ne : if dspac[sp-1].alfval <> dspac[sp].alfval then
                          cc := true
                  end; (* case *)

         ch  : if not istyp(dspac[sp-1].typ,ch,styperr,sp-1) then
                  progstop
               else case relat of
                  gt : if dspac[sp-1].charval > dspac[sp].charval then
                          cc := true;
                  lt : if dspac[sp-1].charval < dspac[sp].charval then
                          cc :=true;
                  ge : if dspac[sp-1].charval >= dspac[sp].charval then
                          cc := true;
                  le : if dspac[sp-1].charval <= dspac[sp].charval then
                          cc := true;
                  eq : if dspac[sp-1].charval = dspac[sp].charval then
                          cc := true;
                  ne : if dspac[sp-1].charval <> dspac[sp].charval then
                          cc := true
                  end; (* case *)

         bool: if not istyp(dspac[sp-1].typ,bool,styperr,sp-1) then
                  progstop
               else case relat of
                  gt : if dspac[sp-1].boolval > dspac[sp].boolval then
                          cc := true;
                  lt : if dspac[sp-1].boolval < dspac[sp].boolval then
                          cc := true;
                  ge : if dspac[sp-1].boolval >= dspac[sp].boolval then
                          cc := true;
                  le : if dspac[sp-1].boolval <= dspac[sp].boolval then
                          cc := true;
                  eq : if dspac[sp-1].boolval = dspac[sp].boolval then
                          cc := true;
                  ne : if dspac[sp-1].boolval <> dspac[sp].boolval then
                          cc := true
                  end; (* case *)
         und : begin
                  writeln(erreltyp);
                  progstop
                  end
               end; (* case *)
      sp := sp - 1;
      dspac[sp].typ := bool;
      dspac[sp].boolval := cc;
      regs[spreg] := sp
      end; (* dorels *)



(**********)
(*   doret - execute the return instruction *)
(**********)

procedure doret;
   const
      raddoffset = 2;   (* offset of return address from dp *)
   var
      dp : integer;
      i : integer;
   begin
      dp := regs[dpreg];
      if not inrange(dp,minstack,maxdspac,stackerr) then
         progstop;
      if not istyp(dspac[dp].typ,int,styperr,dp) then
         progstop;
      i := dspac[dp].intval;
      if not inrange(i+1,minstack,maxdspac,stackerr) then
         progstop;
      dspac[i+1].typ := int;      (* reset forward pointer of *)
      dspac[i+1].intval := 0;     (* calling routine *)
      i := dp + raddoffset;
      if not inrange(i,minstack,maxdspac,stackerr) then
         progstop;
      if not istyp(dspac[i].typ,int,styperr,i) then
         progstop;
      iptr := dspac[i].intval;
      regs[dpreg] := dspac[dp].intval;
      regs[spreg] := dp-1
      end; (* doret *)



(**********)
(*   doclose - execute the close system function *)
(**********)

procedure doclose;
   var
      flocn : integer;
      i : integer;
      sp : integer;
      talf : alfa;
   begin
      sp := regs[spreg];
      if not inrange(sp,minstack,maxdspac,stackerr) then
         progstop;
      if not istyp(dspac[sp].typ,alf,styperr,sp) then
         progstop;
      talf := dspac[sp].alfval;
      fixflename(talf);
      flocn := fsearch(talf);
      if flocn = 0 then
         writeln(warnfile,talf,errfilno)
      else begin
         anamptr := anamptr - 1;
         if not inrange(anamptr,minfiles,maxfiles,afilerr) then begin
            writeln(errprog);
	    progstop
            end;
         anames[anamptr] := fnames[flocn];
         for i := flocn to fnamptr - 2 do
            fnames[i] := fnames[i+1];
         fnamptr := fnamptr - 1
         end;
      regs[spreg] := sp - 1
      end; (* doclose *)



(**********)
(*   dofileops - do file system functions *)
(**********)

procedure dofileops(fnct:integer);
   var
      sp : integer;
      fptr : integer;
      itemp : integer;
      rtemp : real;
      chtemp : char;
      flocn : integer;
      talf : alfa;
   begin
      sp := regs[spreg];
      if fnct in [writint,writreal,writbuf,linelim] then
         fptr := sp - 1
      else
         fptr := sp;
      if not inrange(fptr,minstack,maxdspac,stackerr) then
         progstop;
      if not istyp(dspac[fptr].typ,alf,styperr,fptr) then
         progstop;
      talf := dspac[fptr].alfval;
      fixflename(talf);
      flocn := fsearch(talf);
      if flocn = 0 then begin
         writeln(errfile1,talf,errfilno);
         progstop
         end;
      case fnct of
         getf    : begin
                      case (fnames[flocn].aname[2]) of
                         '1' : doget(t1);
                         '2' : doget(t2);
                         '3' : doget(t3);
                         '4' : doget(t4);
                         '5' : doget(t5);
                         '6' : doget(t6)
                         end; (* case *)
                      sp := sp - 1
                      end;

         putf    : begin
                      case (fnames[flocn].aname[2]) of
                         '1' : put(t1);
                         '2' : put(t2);
                         '3' : put(t3);
                         '4' : put(t4);
                         '5' : put(t5);
                         '6' : put(t6)
                         end; (* case *)
                      sp := sp - 1
                      end;

         readlin : begin
                      case (fnames[flocn].aname[2]) of
                         '1' : doreadln(t1);
                         '2' : doreadln(t2);
                         '3' : doreadln(t3);
                         '4' : doreadln(t4);
                         '5' : doreadln(t5);
                         '6' : doreadln(t6)
                         end; (* case *)
                      sp := sp - 1
                      end;

         writlin : begin
                      addline(flocn);
                      case (fnames[flocn].aname[2]) of
                         '1' : writeln(t1);
                         '2' : writeln(t2);
                         '3' : writeln(t3);
                         '4' : writeln(t4);
                         '5' : writeln(t5);
                         '6' : writeln(t6)
                         end; (* case *)
                      sp := sp - 1
                      end;

         readbuf : begin
                      case (fnames[flocn].aname[2]) of
                         '1' : dobuf(chtemp,t1);
                         '2' : dobuf(chtemp,t2);
                         '3' : dobuf(chtemp,t3);
                         '4' : dobuf(chtemp,t4);
                         '5' : dobuf(chtemp,t5);
                         '6' : dobuf(chtemp,t6)
                         end; (* case *)
                      dspac[sp].typ := ch;
                      dspac[sp].charval := chtemp
                      end;

         writbuf : begin
                      if not inrange(sp,minstack,maxdspac,stackerr) then
                         progstop;
                      if not istyp(dspac[sp].typ,ch,styperr,sp) then
                         progstop;
                      case (fnames[flocn].aname[2]) of
                         '1' : t1^ := dspac[sp].charval;
                         '2' : t2^ := dspac[sp].charval;
                         '3' : t3^ := dspac[sp].charval;
                         '4' : t4^ := dspac[sp].charval;
                         '5' : t5^ := dspac[sp].charval;
                         '6' : t6^ := dspac[sp].charval
                         end; (* case *)
                      sp := sp - 2
                      end;

         readi   : begin
                      case (fnames[flocn].aname[2]) of
                         '1' : doint(t1,itemp);
                         '2' : doint(t2,itemp);
                         '3' : doint(t3,itemp);
                         '4' : doint(t4,itemp);
                         '5' : doint(t5,itemp);
                         '6' : doint(t6,itemp)
                         end; (* case *)
                      dspac[sp].typ := int;
                      dspac[sp].intval := itemp
                      end;

         writint : begin
                      if not inrange(sp,minstack,maxdspac,stackerr) then
                         progstop;
                      if not istyp(dspac[sp].typ,int,styperr,sp) then
                         progstop;
                      case (fnames[flocn].aname[2]) of
                         '1' : write(t1,dspac[sp].intval);
                         '2' : write(t2,dspac[sp].intval);
                         '3' : write(t3,dspac[sp].intval);
                         '4' : write(t4,dspac[sp].intval);
                         '5' : write(t5,dspac[sp].intval);
                         '6' : write(t6,dspac[sp].intval)
                         end; (* case *)
                      sp := sp - 2
                      end;

         readr   : begin
                      case (fnames[flocn].aname[2]) of
                         '1' : doreal(t1,rtemp);
                         '2' : doreal(t2,rtemp);
                         '3' : doreal(t3,rtemp);
                         '4' : doreal(t4,rtemp);
                         '5' : doreal(t5,rtemp);
                         '6' : doreal(t6,rtemp)
                         end; (* case *)
                      dspac[sp].typ := re;
                      dspac[sp].realval := rtemp
                      end;

         writreal: begin
                      if not inrange(sp,minstack,maxdspac,stackerr) then
                         progstop;
                      if not istyp(dspac[sp].typ,re,styperr,sp) then
                         progstop;
                      case (fnames[flocn].aname[2]) of
                         '1' : write(t1,dspac[sp].realval);
                         '2' : write(t2,dspac[sp].realval);
                         '3' : write(t3,dspac[sp].realval);
                         '4' : write(t4,dspac[sp].realval);
                         '5' : write(t5,dspac[sp].realval);
                         '6' : write(t6,dspac[sp].realval)
                         end;
                      sp := sp - 2
                      end;

         endoflin: begin
                      dspac[sp].typ := bool;
                      case (fnames[flocn].aname[2]) of
                         '1' : dspac[sp].boolval := eoln(t1);
                         '2' : dspac[sp].boolval := eoln(t2);
                         '3' : dspac[sp].boolval := eoln(t3);
                         '4' : dspac[sp].boolval := eoln(t4);
                         '5' : dspac[sp].boolval := eoln(t5);
                         '6' : dspac[sp].boolval := eoln(t6)
                         end (* case *)
                      end;

         endoffil: begin
                      dspac[sp].typ := bool;
                      case (fnames[flocn].aname[2]) of
                         '1' : dspac[sp].boolval := eof(t1);
                         '2' : dspac[sp].boolval := eof(t2);
                         '3' : dspac[sp].boolval := eof(t3);
                         '4' : dspac[sp].boolval := eof(t4);
                         '5' : dspac[sp].boolval := eof(t5);
                         '6' : dspac[sp].boolval := eof(t6)
                         end (* case *)
                      end;

         linelim : begin
                      if not inrange(sp,minstack,maxdspac,stackerr) then
                         progstop;
                      if not istyp(dspac[sp].typ,int,styperr,sp) then
                         progstop;
                      fnames[flocn].linelimt := dspac[sp].intval;
                      sp := sp - 2
                      end
                   end; (* case *)
      regs[spreg] := sp
      end; (* dofileops *)



(**********)
(*   doopen - execute the open system function *)
(**********)

procedure doopen;
   var
      sp : integer;
      talf : alfa;
   procedure opener(var fil : text);
      begin
         if dspac[sp].intval = 0 then
	    reset(fil, talf)
         else
	    rewrite(fil, talf)
         end; (* opener *)

   begin (* doopen *)
      sp := regs[spreg];
      if not inrange(sp,minstack,maxdspac,stackerr) then
         progstop;
      if not inrange(sp-1,minstack,maxdspac,stackerr) then
         progstop;
      if not istyp(dspac[sp].typ,int,styperr,sp) then
         progstop;
      if not istyp(dspac[sp-1].typ,alf,styperr,sp-1) then
         progstop;
      talf := dspac[sp-1].alfval;
      fixflename(talf);
      if fsearch(talf) <> 0 then begin
         writeln(errfile1,talf,errfilop);
         progstop
         end;
      fixflename(outname);
      if talf = outname then begin
         writeln(erroutput);
         progstop
         end;
      if not inrange(fnamptr,minfiles,maxfiles,filerr) then
         progstop;
      if not inrange(anamptr,minfiles,maxfiles,afilerr) then
         progstop;
      fnames[fnamptr] := anames[anamptr];
      case (fnames[fnamptr].aname[2]) of
         '1' : opener(t1);
         '2' : opener(t2);
         '3' : opener(t3);
         '4' : opener(t4);
         '5' : opener(t5);
         '6' : opener(t6)
         end (* case *);
      fnames[fnamptr].fname := talf;
      fnames[fnamptr].linenum := 0;
      fnames[fnamptr].linelimt := maxint;
      fnamptr := fnamptr + 1;
      anamptr := anamptr + 1;
      regs[spreg] := sp - 2
      end; (* doopen *)



(**********)
(*   dosyscall - execute the system functions *)
(**********)

procedure dosyscall(* fnct : integer *);
   var
      sp : integer;
      itemp : integer;
      chtemp : char;
   begin
      if (fnct < minfnct) or (fnct > maxfnct) then begin
         writeln(errfunct);
         progstop
         end;
      case fnct of
         opin   : doopen; (* jda *)

         close  : doclose;

         getf,putf,readlin,writlin,readbuf,writbuf,readi,
         writint,readr,writreal,endoflin,endoffil,linelim:
                  dofileops(fnct);

         clockit: begin
                     sp := regs[spreg] + 1;
                     if not inrange(sp,minstack,maxdspac,stackerr)
                        then progstop;
                     dspac[sp].typ := int;
		(*   dspac[sp].intval := clock;          *)
		     dspac[sp].intval := 0;       (* JDA *)
                     regs[spreg] := sp
                     end;

         dumpit : begin
                     dumpregs(minregs,maxregs);
                     dumpdspace(mindspace,maxdspac,dhead);
                     dumpispace(minispace,maxispace)
                     end;

         chrof  : begin
                     sp := regs[spreg];
                     if not inrange(sp,minstack,maxdspac,stackerr)
                        then progstop;
                     if not istyp(dspac[sp].typ,int,styperr,sp) then
                        progstop;
                     itemp := dspac[sp].intval;
                     if (itemp < minordch) or (itemp > maxordch)
                     then begin
                        writeln(errchr);
                        progstop
                        end;
                     dspac[sp].typ := ch;
                     dspac[sp].charval := chr(itemp)
                     end;
         ordof  : begin
                     sp := regs[spreg];
                     if not inrange(sp,minstack,maxdspac,stackerr)
                        then progstop;
                     if not istyp(dspac[sp].typ,ch,styperr,sp) then
                        progstop;
                     chtemp := dspac[sp].charval;
                     dspac[sp].typ := int;
                     dspac[sp].intval := ord(chtemp)
                     end;
         sqrof  : begin
                     sp := regs[spreg];
                     if not inrange(sp,minstack,maxdspac,stackerr)
                        then progstop;
                     if dspac[sp].typ = int then
                        dspac[sp].intval := sqr(dspac[sp].intval)
                     else if dspac[sp].typ = re then
                        dspac[sp].realval := sqr(dspac[sp].realval)
                     else begin
                        writeln(errsqr);
                        progstop
                        end
                     end;
         dumpr,dumpi,dumpd :
                  dodumps(fnct);

         instlim: begin
                       sp := regs[spreg];
                       if not inrange(sp,minstack,maxdspac,stackerr)
                          then progstop;
                       if not istyp(dspac[sp].typ,int,styperr,sp)
                          then progstop;
                       maxinst := dspac[sp].intval;
                       regs[spreg] := sp-1
                       end;
          end; (* case *)
      end; (* dosyscall *)



(**********)
(*   effaddr - calculate the effective address *)
(**********)

function effaddr(* (op : integer): integer *);
   const
      idigmod = 100000000;
      rdigmod = 10000000;
      xmod    = 100000;
   var
      idig : integer;
      rdig : integer;
      xdig : integer;
      ydig : integer;
      t1   : integer;
   begin
      if op < 0 then begin
         writeln(erraddneg);
         progstop
         end;
      idig := op div idigmod;
      op := op mod idigmod;
      rdig := op div rdigmod;
      op := op mod rdigmod;
      xdig := op div xmod;
      op := op mod xmod;
      ydig := op;
      if idig = 0 then begin
         diradds := diradds + 1;
         effaddr := regs[rdig] + ydig
         end
      else begin
         indadds := indadds + 1;
         t1 := regs[rdig] + xdig;
         if dspac[t1].typ <> int then begin
            writeln(erradd1,t1:1,erradd2);
            progstop
            end;
         effaddr := dspac[t1].intval + ydig
         end
      end; (* effaddr *)



(**********)
(*   execute - execute an instruction *)
(**********)

procedure execute;
   var
      swaptemp : datawords;
      paktemp  : alfa;
      curop : integer;
      op1 : integer;
      op2 : integer;
      i : integer;
      rtemp : real;
      itemp : integer;
      sp : integer;
   begin
      curop := curinst.opcode;
      rtemp := 0.0;
      itemp := 0;
      case curop of

      badopc : begin
                  writeln(errbadop);
                  progstop
                  end;
      push :  begin
                 op1 := effaddr(getop1(curinst.oper1));
                 op2 := curinst.oper2;
                 if op2 <= 0 then
                    writeln(warnpush);
                 if op1 = 0 then
                    sp := regs[spreg] + op2
                 else begin
                    sp := regs[spreg];
                    if not inrange(op1,mindspace,maxdspac,dspacerr)
                       then progstop;
                    if not inrange(op1+op2-1,mindspace,maxdspac,
                       dspacerr) then progstop;
                    if not inrange(sp+1,minstack,maxdspac,stackerr)
                       then progstop;
                    if not inrange(sp+op2,minstack,maxdspac,stackerr)
                       then progstop;
                    for i := 0 to op2-1 do begin
                       if istyp(dspac[op1+i].typ,und,dtyperr,op1+i)
                          then begin
                             regs[spreg] := sp;
                             progstop
                             end;
                       sp := sp + 1;
                       dspac[sp] := dspac[op1+i]
                       end
                    end;
                 regs[spreg] := sp
                 end;

      pushr : begin
                 op1 := getop1(curinst.oper1);
                 sp := regs[spreg] + 1;
                 if not inrange(op1,minregs,maxregs,regserr) then
                    progstop;
                 if not inrange(sp,minstack,maxdspac,stackerr) then
                    progstop;
                 dspac[sp].typ := int;
                 dspac[sp].intval := regs[op1];
                 regs[spreg] := sp
                 end;

      pushi : begin
                 if curinst.oper1.typ = und then begin
                    writeln(erropund);
                    progstop
                    end;
                 sp := regs[spreg] + 1;
                 if not inrange(sp,minstack,maxdspac,stackerr) then
                    progstop;
                 dspac[sp] := curinst.oper1;
                 regs[spreg] := sp
                 end;

      pushc : begin
                 op1 := effaddr(getop1(curinst.oper1));
                 op2 := curinst.oper2;
                 sp := regs[spreg] + 1;
                 if not inrange(op1,mindspace,maxdspac,dspacerr) then
                    progstop;
                 if not inrange(sp,minstack,maxdspac,stackerr) then
                    progstop;
                 if not istyp(dspac[op1].typ,alf,dtyperr,op1) then
                    progstop;
                 dspac[sp].typ := ch;
                 dspac[sp].charval := dspac[op1].alfval[op2];
                 regs[spreg] := sp
                 end;

      pop  :  begin
               op1 := effaddr(getop1(curinst.oper1));
               op2 := curinst.oper2;
               sp := regs[spreg];
               if not inrange(sp,minstack,maxdspac,stackerr) then
                  progstop;
               if not inrange(sp-op2,minstack,maxdspac,stackerr)
                  then progstop;
               if op1 = 0 then
                  regs[spreg] := sp - op2
               else begin
                  if not inrange(op1,mindspace,maxdspac,dspacerr)
                     then progstop;
                  if not inrange(op1+op2-1,mindspace,maxdspac,
                     dspacerr) then progstop;
                  for i := op2 - 1 downto 0 do begin
                     if istyp(dspac[sp].typ,und,styperr,sp) then
                     begin
                        regs[spreg] := sp;
                        progstop
                        end;
                     dspac[op1+i] := dspac[sp];
                     sp := sp - 1
                     end;
                  regs[spreg] := sp
                  end
               end;

      popc :  begin
                 op1 := effaddr(getop1(curinst.oper1));
                 op2 := curinst.oper2;
                 sp := regs[spreg];
                 if not inrange(op1,mindspace,maxdspac,dspacerr) then
                    progstop;
                 if not inrange(sp,minstack,maxdspac,stackerr) then
                    progstop;
                 if not istyp(dspac[sp].typ,alf,styperr,sp) then
                    progstop;
                 dspac[op1].typ := ch;
                 dspac[op1].charval := dspac[sp].alfval[op2];
                 regs[spreg] := sp - 1
                 end;

      popr :  begin
                 op1 := getop1(curinst.oper1);
                 sp := regs[spreg];
                 if not inrange(sp,minstack,maxdspac,stackerr) then
                    progstop;
                 if not inrange(op1,minregs,maxregs,regserr) then
                    progstop;
                 if not istyp(dspac[sp].typ,int,styperr,sp) then
                    progstop;
                 if op1 <> 0 then
                    regs[op1] := dspac[sp].intval;
                 regs[spreg] := sp - 1
                 end;

      move :  begin
                 op1 := effaddr(getop1(curinst.oper1));
                 op2 := effaddr(curinst.oper2);
                 if not inrange(op1,mindspace,maxdspac,dspacerr) then
                    progstop;
                 if not inrange(op2,mindspace,maxdspac,dspacerr) then
                    progstop;
                 if istyp(dspac[op2].typ,und,dtyperr,op2) then
                    progstop;
                 dspac[op1] := dspac[op2]
                 end;

      swap :  begin
                 sp := regs[spreg];
                 if not inrange(sp,minstack,maxdspac,stackerr) then
                    progstop;
                 if not inrange(sp-1,minstack,maxdspac,stackerr) then
                    progstop;
                 swaptemp := dspac[sp];
                 dspac[sp] := dspac[sp-1];
                 dspac[sp-1] := swaptemp
                end;

      load :  begin
                 op1 := getop1(curinst.oper1);
                 op2 := effaddr(curinst.oper2);
                 if not inrange(op1,minregs,maxregs,regserr) then
                    progstop;
                 if not inrange(op2,mindspace,maxdspac,dspacerr) then
                    progstop;
                 if not istyp(dspac[op2].typ,int,dtyperr,op2) then
                    progstop;
                 if op1 <> 0 then
                    regs[op1] := dspac[op2].intval
                 end;

      loadr : begin
                 op1 := getop1(curinst.oper1);
                 op2 := curinst.oper2;
                 if not inrange(op1,minregs,maxregs,regserr) then
                    progstop;
                 if not inrange(op2,minregs,maxregs,regserr) then
                    progstop;
                 if op1 <> 0 then
                    regs[op1] :=  regs[op2]
                 end;

      loada : begin
                 op1 := getop1(curinst.oper1);
                 op2 := effaddr(curinst.oper2);
                 if not inrange(op1,minregs,maxregs,regserr) then
                    progstop;
                 if op1 <> 0 then
                    regs[op1] := op2
                 end;

      loadi : begin
                 op1 := getop1(curinst.oper1);
                 op2 := curinst.oper2;
                 if not inrange(op1,minregs,maxregs,regserr) then
                    progstop;
                 if op1 <> 0 then
                    regs[op1] := op2
                 end;

      store : begin
                 op1 := getop1(curinst.oper1);
                 op2 := effaddr(curinst.oper2);
                 if not inrange(op1,minregs,maxregs,regserr) then
                    progstop;
                 if not inrange(op2,mindspace,maxdspac,dspacerr)
                     then progstop;
                 dspac[op2].typ := int;
                 dspac[op2].intval := regs[op1]
                 end;

      stregs: begin
                 op1 := effaddr(getop1(curinst.oper1));
                 op2 := curinst.oper2;
                 if not inrange(op1,mindspace,maxdspac,dspacerr)
                    then progstop;
                 if not inrange(op1+op2-1,mindspace,maxdspac,
                    dspacerr) then progstop;
                 for i := 0 to op2-1 do begin
                    dspac[op1+i].typ := int;
                    dspac[op1+i].intval := regs[(i+3) mod 10]
                    end
                 end;

      ldregs: begin
                 op1 := effaddr(getop1(curinst.oper1));
                 op2 := curinst.oper2;
                 if not inrange(op1,mindspace,maxdspac,dspacerr) then
                    progstop;
                 if not inrange(op1+op2-1,mindspace,maxdspac,dspacerr)
                    then progstop;
                 for i := 0 to op2-1 do begin
                    if not istyp(dspac[op1+i].typ,int,dtyperr,op1+i)
                       then progstop;
                     regs[(i+3) mod 10] := dspac[op1+i].intval
                     end
                  end;


      haltt : done := true;

      add,sub,mult,divide :
              domath(curop);

      modulus:begin
                 sp := regs[spreg] - 1;
                 if not inrange(sp,minstack,maxdspac,stackerr) then
                    progstop;
                 if not inrange(sp+1,minstack,maxdspac,stackerr) then
                    progstop;
                 if not istyp(dspac[sp].typ,int,styperr,sp) then
                    progstop;
                 if not istyp(dspac[sp+1].typ,int,styperr,sp+1) then
                    progstop;
                 if dspac[sp+1].intval = 0 then begin
                    writeln(errdivide);
                    progstop
                    end;
                 if abs(dspac[sp+1].intval) > maxint then begin
                    writeln(intoverflow);
                    dspac[sp+1].intval := maxint
                    end;
                 if abs(dspac[sp].intval) > maxint then begin
                    writeln(intoverflow);
                    dspac[sp].intval := maxint
                    end;
                 dspac[sp].intval := dspac[sp].intval mod
                    dspac[sp+1].intval;
                 regs[spreg] := sp
                 end;

      orop,andop : begin
                 sp := regs[spreg] - 1;
                 if not inrange(sp,minstack,maxdspac,stackerr) then
                    progstop;
                 if not inrange(sp+1,minstack,maxdspac,stackerr) then
                    progstop;
                 if not istyp(dspac[sp].typ,bool,styperr,sp) then
                    progstop;
                 if not istyp(dspac[sp+1].typ,bool,styperr,sp+1) then
                    progstop;
                 if curop = orop then
                    dspac[sp].boolval := dspac[sp].boolval or
                       dspac[sp+1].boolval
                 else
                    dspac[sp].boolval := dspac[sp].boolval and
                       dspac[sp+1].boolval;
                 regs[spreg] := sp
                 end;

      notop : begin
                 sp := regs[spreg];
                 if not inrange(sp,minstack,maxdspac,stackerr) then
                    progstop;
                 if not istyp(dspac[sp].typ,bool,styperr,sp) then
                    progstop;
                 dspac[sp].boolval := not dspac[sp].boolval
                 end;

      negate: begin
                 sp := regs[spreg];
                 if not inrange(sp,minstack,maxdspac,stackerr) then
                    progstop;
                 op1 := getop1(curinst.oper1);
                 if op1 = 0 then
                    if not istyp(dspac[sp].typ,int,styperr,sp) then
                       progstop
                    else
                       dspac[sp].intval := - dspac[sp].intval
                 else
                    if not istyp(dspac[sp].typ,re,styperr,sp) then
                       progstop
                    else
                       dspac[sp].realval := - dspac[sp].realval
                 end;

      roundop,truncop : begin
                 op1 := getop1(curinst.oper1);
                 sp := regs[spreg];
                 if op1 <> 0 then
                    sp := sp - 1;
                 if not inrange(sp,minstack,maxdspac,stackerr) then
                    progstop;
                 if not istyp(dspac[sp].typ,re,styperr,sp) then
                    progstop;
                 rtemp := dspac[sp].realval;
                 dspac[sp].typ := int;
                 if curop = truncop then
                    dspac[sp].intval := trunc(rtemp)
                 else
                    dspac[sp].intval := round(rtemp)
                 end;

      float : begin
                 op1 := getop1(curinst.oper1);
                 sp := regs[spreg];
                 if op1 <> 0 then
                    sp := sp - 1;
                 if not inrange(sp,minstack,maxdspac,stackerr) then
                    progstop;
                 if not istyp(dspac[sp].typ,int,styperr,sp) then
                    progstop;
                 itemp := dspac[sp].intval;
                 dspac[sp].typ := re;
                 dspac[sp].realval := itemp
                 end;

      shift : begin
                 op1 := getop1(curinst.oper1);
		 op2 := curinst.oper2 mod 32;    (* JDA *)
                 sp := regs[spreg];
                 if not inrange(sp,minstack,maxdspac,stackerr) then
                    progstop;
                 if not istyp(dspac[sp].typ,int,styperr,sp) then
                    progstop;
                 itemp := dspac[sp].intval;
                 if op1 = 0 then
                    for i := 1 to op2 do
                       itemp := itemp * 2
                 else
                    for i := 1 to op2 do
                       itemp := itemp div 2;
                 dspac[sp].intval := itemp
                 end;

      gt,lt,ge,le,eq,ne :
                 dorels(curop);

      b :     begin
                 op1 := effaddr(getop1(curinst.oper1));
                 iptr := op1
                 end;

      bct :   begin
                 sp := regs[spreg];
                 if not inrange(sp,minstack,maxdspac,stackerr) then
                    progstop;
                 if not istyp(dspac[sp].typ,bool,styperr,sp) then
                    progstop;
                 if dspac[sp].boolval then
                    iptr := effaddr(getop1(curinst.oper1));
                 regs[spreg] := sp - 1
                 end;

      noop :  ;

      pak   : begin
                 sp := regs[spreg];
                 if not inrange(sp,minstack,maxdspac,stackerr) then
                    progstop;
                 if not inrange(sp-9,minstack,maxdspac,stackerr)
                    then progstop;
                 for i := 10 downto 1 do begin
                    if not istyp(dspac[sp].typ,ch,styperr,sp) then
                       begin
                       regs[spreg] := sp;
                       progstop
                       end;
                    paktemp[i] := dspac[sp].charval;
                    sp := sp - 1
                    end;
                 sp := sp + 1;
                 dspac[sp].typ := alf;
                 dspac[sp].alfval := paktemp;
                 regs[spreg] := sp
                 end;

      unpak : begin
                 sp := regs[spreg];
                 if not inrange(sp,minstack,maxdspac,stackerr) then
                    progstop;
                 if not inrange(sp+9,minstack,maxdspac,stackerr)
                    then progstop;
                 if not istyp(dspac[sp].typ,alf,styperr,sp) then
                    progstop;
                 paktemp := dspac[sp].alfval;
                 for i := 10 downto 1 do begin
                    dspac[sp].typ := ch;
                    dspac[sp].charval := paktemp[i];
                    sp := sp + 1
                    end;
                 regs[spreg] := sp - 1
                 end;

      syscall:begin
                 op1 := getop1(curinst.oper1);
                 dosyscall(op1)
                 end;

      call :  begin
                 op1 := getop1(curinst.oper1);
                 op2 := effaddr(curinst.oper2);
                 docall(op1,op2)
                 end;

      return: doret;

      bcf   : begin
                 sp := regs[spreg];
                 if not inrange(sp,minstack,maxdspac,stackerr) then
                    progstop;
                 if not istyp(dspac[sp].typ,bool,styperr,sp) then
                    progstop;
                 if not dspac[sp].boolval then
                    iptr := effaddr(getop1(curinst.oper1));
                 regs[spreg] := sp - 1
                 end
         end; (* case *)
      end; (* execute *)



(**********)
(*   fetch - fetch the next instruction *)
(**********)

procedure fetch;
   begin
      if not inrange(iptr,minispace,maxispace,ispacerr) then
         progstop;
      if instnumb >= maxinst then begin
         write(ilimerr1); intwrite(instnumb); write(ilimerr2);
         intwrite(maxinst); writeln;
         progstop
         end;
      curinst := ispace[iptr];
      icache[icacheptr] := curinst;
      icacheptr := (icacheptr + 1) mod icachesize;
      iptr := iptr + 1;
      instnumb := instnumb + 1
      end; (* fetch *)



(**********)
(*   fsearch - find index in fnames of file fnm *)
(**********)

function fsearch(* (fnm : alfa ): integer *);
   var
      f : integer;
   begin
      f := fnamptr - 1;
      fnames[0].fname := fnm;
      while fnames[f].fname <> fnm do
         f := f - 1;
      fsearch := f
      end;



(**********)
(*   getop1 - get the first operand, if it is an integer *)
(**********)

function getop1(* (op1 : datawords): integer *);
   const
      errtyp = '0 error - type error, oper1 must be integer';
   begin
      if op1.typ <> int then begin
         writeln(errtyp);
         progstop
         end;
      getop1 := op1.intval
      end; (* getop1 *)



(**********)
(*   initialize - do the initializations *)
(**********)

procedure initialize;
   var
      i : integer;
   begin
      writeln('1',initmes);
      opcodenames[badopc]  := '        0.';
      opcodenames[push]    := '     push.';
      opcodenames[pushr]   := '    pushr.';
      opcodenames[pushi]   := '    pushi.';
      opcodenames[pushc]   := '    pushc.';
      opcodenames[pop]     := '      pop.';
      opcodenames[popc]    := '     popc.';
      opcodenames[popr]    := '     popr.';
      opcodenames[move]    := '     move.';
      opcodenames[load]    := '     load.';
      opcodenames[loada]   := '    loada.';
      opcodenames[loadi]   := '    loadi.';
      opcodenames[loadr]   := '    loadr.';
      opcodenames[store]   := '    store.';
      opcodenames[stregs]  := '   stregs.';
      opcodenames[ldregs]  := '   ldregs.';
      opcodenames[haltt]   := '    haltt.';
      opcodenames[add]     := '      add.';
      opcodenames[sub]     := '      sub.';
      opcodenames[mult]    := '     mult.';
      opcodenames[divide]  := '   divide.';
      opcodenames[modulus] := '  modulus.';
      opcodenames[orop]    := '     orop.';
      opcodenames[andop]   := '    andop.';
      opcodenames[notop]   := '    notop.';
      opcodenames[negate]  := '   negate.';
      opcodenames[truncop] := '  truncop.';
      opcodenames[roundop] := '  roundop.';
      opcodenames[float]   := '    float.';
      opcodenames[shift]   := '    shift.';
      opcodenames[gt]      := '       gt.';
      opcodenames[ge]      := '       ge.';
      opcodenames[lt]      := '       lt.';
      opcodenames[le]      := '       le.';
      opcodenames[eq]      := '       eq.';
      opcodenames[ne]      := '       ne.';
      opcodenames[b]       := '        b.';
      opcodenames[bct]     := '      bct.';
      opcodenames[bcf]     := '      bcf.';
      opcodenames[noop]    := '     noop.';
      opcodenames[syscall] := '  syscall.';
      opcodenames[call]    := '     call.';
      opcodenames[return]  := '   return.';
      opcodenames[swap]    := '     swap.';
      opcodenames[pak]     := '     pack.';
      opcodenames[unpak]   := '   unpack.';
      anamptr := 1;
      fnamptr := 1;
      anames[1].aname := 't1        ';
      anames[2].aname := 't2        ';
      anames[3].aname := 't3        ';
      anames[4].aname := 't4        ';
      anames[5].aname := 't5        ';
      anames[6].aname := 't6        ';
      outname := 'output    ';
      fixflename(outname);
      icacheptr := 0;
      numbset := ['0'..'9'];
      for i := minispace to maxispace do
         with ispace[i] do begin
            opcode := 0;
            oper1.typ:= int;
            oper1.intval := 0;
            oper2 := 0
            end;
      for i := 0 to 9 do
         with icache[i] do begin
            opcode := 0;
            oper1.typ := int;
            oper1.intval := 0;
            oper2 := 0
            end;
      for i := mindspace to maxdspac do begin
         dspac[i].typ := und;
         dspac[i].undval := ' undefined'
         end;
      for i := minregs to maxregs do
         regs[i] := 0;
      maxinst := maxint;
      iptr := -1;
      indadds := 0;
      diradds := 0;
      instnumb := 0
      end; (* initialize *)




(**********)
(*   inrange - true if element is included in bot..top *)
(**********)

function inrange(* (element,bot,top : integer;           *)
                (*  msg : message): boolean              *);
   begin
      inrange := true;
      if (element < bot) or (element > top) then begin
         inrange := false;
         writeln(msg,element)
         end
      end; (* inrange *)



(**********)
(*   intwrite - write an integer and check for integer overflow *)
(**********)

procedure intwrite;
   const
      intover = ' integer overflow ';
   begin
      if i > maxint then
         write(intover)
      else
         write(i)
      end; (* intwrite *)



(**********)
(*   istyp - returns true if the two typed arguments agree *)
(**********)

function istyp(* (element : typs; typex : typs; msg : alfa;     *)
              (*  enum : integer) : boolean                     *);
   const
      typerr = '] has wrong type ';
      intexp = ' - integer expected';
      reexp  = ' - real expected';
      chexp  = ' - char expected';
      alfexp = ' - alfa expected';
      blexp  = ' - boolean expected';
   begin
      istyp := true;
      if element <> typex then begin
         istyp := false;
         if typex <> und then
            write('0type error - ',msg,enum:1,typerr);
         case typex of
            int  : writeln(intexp);
            re   : writeln(reexp);
            alf  : writeln(alfexp);
            ch   : writeln(chexp);
            bool : writeln(blexp);
            und  : ;
            end (* case *)
         end
      else if typex = und then
         writeln(msg,enum:1,'] is undefined ')
      end; (* istyp *)



(**********)
(*   progstop - print out a dump and stop the program *)
(**********)

procedure progstop;
   const
      stackdump = 10;
   var
      bot : integer;
      i : integer;
     sp : integer;
   begin
      writeln(machtop);
      write(' iptr is '); intwrite(iptr);
      write('.   number of instructions executed is ');
      intwrite(instnumb); writeln;
      dumpregs(minregs,maxregs);
      if instnumb - 1 <= 0 then begin
         writeln(errfinst);
         dumpispace(minispace,maxispace);
         dumpdspace(mindspace,maxdspac,dhead)
         end
      else begin
         sp := regs[spreg];
         if (sp < mindspace) or (sp > maxdspac) then begin
            writeln(errsp);
            dumpdspace(mindspace,maxdspac,dhead)
            end
         else begin
            if sp >= stackdump then
               bot := sp - (stackdump - 1)
            else
               bot := 1;
            dumpdspace(bot,sp,shead)
            end;
         writeln(cachetop);
         i := icacheptr;
         repeat
            dumpiword(icache[i]);
            i := (i + 1) mod icachesize
         until i = icacheptr;
         dodumpdisp
         end;
      haltexecution
      end; (* progstop *)



(**********)
(*   readfile - load in the loadfile *)
(**********)

procedure readfile;
   const
      iptrreg = 10;
      locerr = '0load error - location out of range     ';
      regerr = '0load error - illegal register index    ';
      spcerr = '0 load error - illegal value for spce   ';
   var
      location : integer;
      index : integer;
   procedure loaderr(errmess : message; locn : integer);
      begin
         write(errmess); intwrite(locn); writeln;
         dumpispace(minispace,maxispace);
         dumpdspace(mindspace,maxdspac,dhead);
         dumpregs(minregs,maxregs);
         writeln(' iptr is ');intwrite(iptr); writeln;
	 haltexecution
         end; (* loaderr *)
   begin
      reset(loadfile, 'loadfile');
      while not eof(loadfile) do begin
         location := loadfile^.loc;
         if not (loadfile^.spce in [ispce,dspce,rgs]) then
            loaderr(spcerr,location);
         case loadfile^.spce of
            ispce : begin
                       if (location < minispace) or
                          (location > maxispace) then
                             loaderr(locerr,location);
                       ispace[location] := loadfile^.ivalue;
                       with ispace[location] do
                          if (opcode < minopcode) or
                             (opcode > maxopcode) then begin
                             writeln(' illegal opcode', opcode,
                                ' loaded at location ',location);
                             opcode := noop
                             end
                       end;
            dspce : begin
                       if (location < mindspace) or
                          (location > maxdspac) then
                             loaderr(locerr,location);
                       dspac[location] := loadfile^.dvalue
                       end;
            rgs   : begin
                       index := loadfile^.rvalue.regnum;
                       if (index < minregs) or
                          (index > iptrreg) then
                             loaderr(regerr,index);
                       if index = iptrreg then
                          iptr := loadfile^.rvalue.regval
                       else if index > 0 then
                          regs[index] := loadfile^.rvalue.regval
                       end
              end; (* case *)
           get(loadfile)
            end
      end; (* readfile *)



(**********)
(*   readint - read an integer off a file *)
(**********)

procedure readint(* var f : text; var ival : integer *);
   var
      sign : boolean;
   begin
      sign := false;
      while f^ = blank do
         get(f);
      if f^ = minus then begin
         sign := true;
         get(f)
         end;
      if not (f^ in numbset) then begin
         writeln(errnumb);
         progstop
         end;
      ival := 0;
      while f^ in numbset do begin
         ival := 10 * ival + ord(f^) - ord('0');
         get(f)
         end;
      if ival > maxint then begin
         writeln(intoverflow);
         ival := maxint
         end;
      if sign then
         ival := - ival
      end; (* readint *)



(**********)
(*   readreal - read a real number off a file *)
(**********)

procedure readreal(* var f : text; var rval : real *);
   const
      maxlen = 79;
   var
      i : integer;
      string : line;
      sign : boolean;
   procedure getelm;
      begin
         string[i] := f^;
         if i < maxlen then
            i := i + 1
         else begin
            writeln(errlong);
            progstop
            end;
         get(f)
         end; (* getelm *)
   begin
      sign := false;
      while f^ = blank do
         get(f);
      if f^ = minus then begin
         sign := true;
         get(f)
         end;
      if not (f^ in numbset) then begin
         writeln(errnumb);
         progstop
         end;
      i := 1;
      while f^ in numbset do
         getelm;
      if f^ = '.' then begin
         getelm;
          while f^ in numbset do
             getelm
          end;
      i := i - 1;
      rval := convrtr(string,i);
      if sign then
         rval := - rval
      end; (* readreal *)



(**********)
(*   tena - convert exponent to internal format        *)
(*   ref. - jensen & wirth, pascal user's manual       *)
(**********)

function tena(* (e : integer) : real *);
   var
      i : integer;
      t : real;
   begin
      i := 0;
      t := 1.0;
      repeat
         if odd(e) then
            case i of
               0 : t := t * 1.0e1;
               1 : t := t * 1.0e2;
               2 : t := t * 1.0e4;
               3 : t := t * 1.0e8;
               4 : t := t * 1.0e16;
               5 : t := t * 1.0e32
          (*   6 : t := t * 1.0e64;
               7 : t := t * 1.0e128;
               8 : t := t * 1.0e256     jda *)
               end;
         e := e div 2;
         i := i + 1
      until e = 0;
      tena := t
      end; (* tena *)



(**********)
(*   terminate - print messages for normal termination *)
(**********)

procedure terminate;
   const
      term1 = '   normal termination ';
      term2 = '   number of indirect addresses : ';
      term3 = '   number of direct addresses : ';
      term4 = '   number of instructions executed : ';
   begin
      writeln(term1);
      writeln(term2:40, indadds:20);
      writeln(term3:40, diradds:20);
      writeln(term4:40, instnumb:20)
      end; (* terminate*)

(**********)
(*    fixflename - set all spaces following file name to null
(**********)

procedure fixflename (* var talf : alfa *);
   var
      i : integer;
   begin
      i := 1;
         while i <= 10 do begin
            if (((talf[i] >= 'a') and (talf[i] <='z')) or
               ((talf[i] >= 'A') and (talf[i] <='Z')) or
               ((talf[i] >= '0') and (talf[i] <='9'))) then
               i := i  (* null stmt - do nothing *)
            else
               talf[i] := chr(0);
            i := i + 1;
         end;
   end; (* fixflename *)



   begin (* main program *)
      initialize;
      readfile;
      minstack := regs[dpreg];
      done := false;
      while not done do begin
         fetch;
         execute
         end;
      terminate;
      end.
