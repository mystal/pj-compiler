program lranal (input, output, parser);

{


                              the lranal program

             the lrpt program has been developed to provide  the  lr
        machines  and parsing tables.  it is given a list of options
        and a grammar from which to develop the machines.   further,
        provision has been made for including semantic routines with
        the productions in the event that the system  were  used  to
        develop real compilers.

             input characteristics

             input to the program is in a fairly  fixed  format.   a
        word  is  a string of non-blank characters.  end-of-line and
        blank lines are ignored.

             the input is  divided  into  four  sections:   options,
        terminals,  nonterminals,  and  productions.   the  keywords
        options, terminals, nonterminals, and productions may be  in
        any  case;   however,  they  must appear in that order, like
        this:
                 options
                 fstfol echo slr table lr
                 terminals
                 i t a e b
                 nonterminals
                 s  ss'  c
                 productions
                 s --> i c t s ss' &
                 s --> a  &
                 ss' --> e s  &
                 ss' -->  &
                 c  --> b  &

        the options section

             any (or all)  of  several  options  may  be  specified:
        echo,  fsm,  fstfol, table,  slr,  lalr,  lr, or min.  initially, no
        options are in effect.

             the  echo  option   specifies   that   the   terminals,
        nonterminals,  productions, and first and follow sets are to
        be  printed   out.    the   terminals,   nonterminals,   and
        productions  are  numbered,  and  the  tables refer to these
        numbers for indices and elements.

             the fsm (for finite  state  machine)  option  specifies
        that  the  lr(0),  lalr(1),  and  lr(1)  machines  are to be
        printed out (provided that the slr, lalr, lr, or min options
        are specified).

             the fstfol (for first and follow) prints the first and
        follow sets for each terminal symbol.

             the table option indicates  that  the  generated  parse
        table   is   to   be  printed,  along  with  a  pascal  type
        specification of the table.

             the slr, lalr, and lr options indicate that the program
        should  calculate whether the grammar is slr(1), lalr(1), or
        lr(1), respectively.  the program tells whether or  not  the
        grammar  is  in the specified class, and prints which states
        (if any) have shift/reduce or reduce/reduce conflicts.

             the min option requests that the  program  first  check
        the  grammar for being slr(1), then for lalr(1), and finally
        lr(1), until a grammar type  is  found  which  generates  no
        conflicts.

        the terminals section

             the terminals section consists of a list  of  terminals
        to be used in the productions.  terminals may not have names
        that duplicate each other or any of the nonterminals.

        the nonterminals section

             the  nonterminals  section  consists  of  a   list   of
        nonterminals  to  be  used in the productions.  nonterminals
        may not have names that duplicate each other or any  of  the
        terminals.

             the first symbol listed in this section is  assumed  to
        be  the unique start  symbol  and  does  not have  to appear
        on the rhs of a production.

        the productions section

             the  productions  section  consists  of   a   list   of
        productions, each of the form:
                lhs --> rhs1 rhs2 ...  rhsn &
        where lhs is nonterminal, the arrow symbol (-->) may be  any
        symbol  (it  is ignored), and rhs1 through rhsn is a list of
        zero or more terminals and/or nonterminals.   the  character
        "&"  must  appear  literally. 
        end-of-line is ignored, so the production  may  extend  over
        several lines.

             all productions for a given left-hand side must  appear
        together,  but these groupings may occur in any order.

             the semantic code is embedded in a procedure  and  case
        statement,  and  printed to the file parser.  it is intended
        that the tables produced by this program and the  parser  in
        the file lrmain.pas can be inserted in that file to create a
        complete lr parser for some compiler. --Disabled JDA--

        things to be careful of

             the lrpt program augments the grammar by creating a new
        terminal  "$"  and  a new nonterminal "s'", and adds them to
        their respective lists.  while  a  program  need  not  worry
        about  the new nonterminal, the scanner should be created so
        that it returns the number corresponding to "$" whenever the
        end of the file being parsed is reached.

             the arrow symbol (the second symbol in a production) is
        ignored, so some symbol must be present.

        error messages

             the constants referred to below are  constants  in  the
        lrpt program.  these can be adjusted if necessary.
        1:  too many symbols.  controlled by constant maxst.
        2:  missing keyword options.
        3:  unrecognized option.
        4:  end of file encountered while reading options.
        5:  missing keyword terminals.
        6:  end of file encountered while reading terminals.
        7:  missing keyword nonterminals.
        8:  end of file encountered while reading nonterminals.
        9:  missing keyword productions.
        25:  too many productions.  controlled by constant maxprods.
        30:  symbol used on left-hand side in neither  terminal  nor
        nonterminal lists.
        31:  symbol used on right-hand side in neither terminal  nor
        nonterminal lists.
        40:  symbol too large to be read in.  controlled by constant
        maxwdlen.
        50:  terminal symbol used on left-hand side.
        51:  terminal symbol never used on right-hand side.
        52:  nonterminal symbol never used on left-hand side.
        53:  nonterminal symbol never used on right-hand side.
        60:  production used out of order;  all with same  left-hand
        side must be grouped together.
        70:  no options are in effect.

        implementation methods used

             the  algorithms  used  in  the  lrpt  program  are  all
        straightforward  implementations of the definitions, and are
        heavily based on [aho77].  internally,  sets  of  items  are
        stored   as   linked  lists.   while  access  is  then  only
        sequential, this does not affect  performance  significantly
        for  many  examinations  of  the sets of items, as they work
        from one end to the other.  the  exception  is  for  testing
        whether   a   set  of  items  is  already  in  the  machine.
        currently, the system tests this by scanning the machine  as
        created so far.

             words read in are stored in a symbol table;  from  then
        on, only their index is used.

             the lrpt program  is  implemented  in  standard  pascal
        (vax-11  pascal  v2.3 running on vax/vms version v3.2).  the
        output tables are in a form basically  compatible  with  vax
        pascal  extended  to  provide  array  constants.  the action
        routine and parser are in standard pascal.

        output

             the terminals, nonterminals, and productions are listed
        (to   output)  along  with  the  numbers  the  parse  tables
        associate with them.  the first and follow functions  (sets)
        are printed for each nonterminal.

             the finite-state machines are printed  (to  output)  by
        showing  the  state  number, the set of items, and a list of
        state numbers for which the goto function is defined.

             a pair of tables are printed to output.  these
        tables  record  the  left-hand side and number of symbols in
        the right-hand side for each production.

             the semantic information is embedded in a procedure and
        copied to the file parser. --Disabled JDA--

             the parse tables are printed (to ouptut) with a  number
        in  braces in each row, indicating with which state that row
        is associated.   the  pascal  type  of  the  table  is  also
        printed.

             finally, it is printed (to output) whether the  grammar
        is  of  the  type specified (slr(1), lalr(1), or lr(1)).  if
        the grammar is not of that type,  a  list  of  conflicts  is
        printed  showing  their type (shift/reduce or reduce/reduce)
        and where in the table they occurred.
}

const

   maxwdlen = 300;   {maximum word length;  actually limited by vax pascal i/o}

   maxst = 254;     {maximum symbol table size}

   maxprods = 1000;  {maximum number of productions}

   emptysym = 0;     {representation of the null string}

   maxpagecols = 80; {maximum number of columns for output on information
                        that might overflow the output buffer}

   
type
      {specify which lr method should be used.}
   lr1type = (slrtype, lalrtype, lrtype);

      {possible options}
   optlist = (fsmopt, minopt, slropt, lalropt, lropt, fstfolopt,
              tblopt, echoopt);

      {definition of a word}
   wordtype = record
      value : array [1..maxwdlen] of char;
      length : 0..maxwdlen;
   end;

      {range definition of symbols;  see definition of symtab below.}
   symbol = 0..maxst;
   symbolsh = 1..maxst;

      {first, follow, and lookahead sets.}
   symset = set of symbol;



{------------}

      {right-hand sides of productions are represented 
         as linked lists over prodlist and prodval.}

   prodlist = ^prodval;

   prodval = record
      sym : symbol;
      next : prodlist;
   end;

      {productions are represented by this structure;  see prodarray below.}
   prodrec = record
      num : integer;      {number of this production}
      lhs : symbol;       {left-hand side symbol}
      rhlist, rhend : prodlist;   {right-hand side pointers to first
                                    and last elements.}
   end;

   production = 0..maxprods;

{--------------------}

      {lr(1) items;  lr(0) items just ignore the sym field.}
   item1 = record
      prod : production;   {production}
      dp : prodlist;       {distinguished position}
      sym : symset;        {lookahead set}
   end;


      {sets of items are a linked list over item1set and itemrec.}
   item1set = ^itemrec;

   itemrec = record
      item : item1;      {item}
      lalr : boolean;    {flag for use in reducing from lr(1) to 
                           lalr(1) machine.}
      next : item1set;   {next set of items on list}
   end;


      {lr machines are a linked list over lrmac and lrmacrec.}
   lrmac = ^lrmacrec;

         {goto-lists are a linked list over goptr and gorec.}
      goptr = ^gorec;

      gorec = record
	 sym : symbol;   {goto state go on reading sym.}
	 go : lrmac;
	 next : goptr;
      end;
   
   lrmacrec = record
      itemset : item1set;      {set of items in lr machine}
      golist : goptr;          {goto-list}
      statenum : integer;      {number of this machine state}
      lalrset : lrmac;         {equivalent lalr state (used in compressing
                                 lr(1) machine for lalr(1)).}
      next : lrmac;            {next state}
   end;

{------------------------}

var
   intnotused: integer; {a dummy variable that is never referenced}
   boolnotused: boolean; {a dummy variable that is never referenced}

   parser : text;   {output file for semantics}

   ch : char;       {last character read}

      {predefined words:  keywords, options, and others}
   lastword, optnwd, termwd, nontermwd, prodnwd, fsmwd, minwd,
     slrwd, lalrwd, lrwd, fstfolwd, tblwd, echowd, eopwd,
     eofwd, dolwd, sprimewd
      : wordtype;

      {current options in effect}
   opts : set of optlist;

      {count of terminals, nonterminals, and productions respectively.}
   numt, numnt, nump : integer;

      {true if no errors have been found.}
   errorfree : boolean;


      {symbol table structure}
   symtab : array [symbol] of record
      word : wordtype;            {symbol}
      isterminal : boolean;       {true if a terminal}
      pos : integer;              {position in symbol table}
      usedlhs,usedrhs : boolean; {true if used on lhs or rhs respectively}
      usedprod : boolean;         {true if in first group of productions
                                    with a given lhs.}
      first, follow : symset;     {first and follow sets.}
   end;

      {current position in symbol table}
   curst : symbol;

      {special symbols}
   startlhssym,          {start symbol}
   dolsym,               (* $ *)
   sprimesym : symbol;   {s'}

      {productions in table form}
   prodarray : array [production] of prodrec;

      {important productions}
   sprimepn,             {production # of "s' --> s"}
   curprod : production; {current production table location}

      {index to production table prodarray.}   
   prodindex : array [symbol] of record
      index : 1..maxprods;   {position of first production with lhs
                                  equal to the index of this table}
      count : integer;       {number of productions with this lhs}
   end;




{   writesym (s : symbol)
      given a symbol, write it to the file output.  return length of symbol 
      written.
}

function writesym (s : symbol) : integer;
   var i : integer;
begin
   writesym := symtab [s].word.length + 1;

   if s = emptysym then
   begin
      write (output, '--empty--');
      writesym := 10;
   end
   else
   for i := 1 to symtab [s].word.length do 
   begin 
      write (output, symtab [s].word.value [i]);
   end; {for} 

   write (output, ' ');

end; {writesym}

{   writesymsh (s, len)
      given a symbol, write it to the file output to maximum length specified.
return length of symbol 0written.
}

function writesymsh (s : symbol; len : integer) : integer;
   var i : integer;
       size : integer;
       symlen : integer;
begin
   symlen := symtab [s].word.length;
   size := symlen;
   if symlen > len then
      size := len;

   for i := 1 to size do 
      write (output, symtab [s].word.value [i]);

   if symlen > size then
      write (output, '~')
   else
      for i := size + 1 to len do
	 write (output, ' ');

   writesymsh := size;
end; {writesymsh}

{   writeset (s : symset, ch : char)
      write the set s to output using ch as a separator character.
}

procedure writeset (s : symset;  ch : char);
   var i : integer;
       col : integer;
       tempset : symset;
begin
   write (output, '[ ');
   tempset := s;
   col := 3;

   for i := emptysym to numt do 
   begin 
      if i in tempset then
      begin 
	 col := col + 2 + writesym (i);
         if col > maxpagecols then 
         begin
            writeln (output);
            col := 2;
         end;

         tempset := tempset - [i];
         if tempset <> [] then
            write (output, ch, ' ');
      end; {if}
   end; {for} 
   
   writeln (output, ' ]');

end; {writeset}

{   wriset (i : item1set, lr0 : boolean)
      given a set of items i, write them to output.  if lr0 is false,
         write the lookahead sets as well.
}

procedure wriset (i : item1set;  lr0 : boolean);
   var scan : item1set;
       dpscan : prodlist;
begin
   writeln (output, '----------------------');
   writeln (output, '    set of items');

   scan := i;
   while scan <> nil do 
   begin 
      write (output, '  ');
      intnotused := writesym (prodarray [scan^.item.prod].lhs);
      write (output, ' --> ');

      dpscan := prodarray [scan^.item.prod].rhlist;
      while dpscan <> scan^.item.dp do 
      begin 
	 intnotused := writesym (dpscan^.sym);
         dpscan := dpscan ^.next;
      end; {while} 

      write (output, ' . ');
      
      while dpscan <> nil do 
      begin 
	 intnotused := writesym (dpscan^.sym);
         dpscan := dpscan ^.next;
      end; {while} 

      if not lr0 then 
      begin 
         writeln (output);
         writeln (output, '  on ');
	 writeset (scan^.item.sym, '/');
      end; {if} 

      writeln (output);
      scan := scan^.next;
   end; {while} 
   writeln (output);
   writeln (output, '--------------------');
end; {wriset}

{   wrmac (m : lrmac, lr0 : boolean)
      write the lr machine m to output.  if lr0 is false, write the
         lookahead sets as well.
}

procedure wrmac (m : lrmac; lr0 : boolean);
   var scan : lrmac;
         goscan : goptr;
begin
   writeln (output, '     ');
   writeln (output, '     ');
   writeln (output, '=== lr machine ===');
   scan := m;

   while scan <> nil do 
   begin 
      writeln (output);
      writeln (output, 'state ', scan^.statenum:4, ': ');
      wriset (scan^.itemset, lr0);
   
      if scan^.golist <> nil then
      begin
	 writeln (output);
	 writeln (output, '   goto transitions');
	 writeln (output);
      end;

      goscan := scan^.golist;
      while goscan <> nil do 
      begin 
	 intnotused := writesym (goscan^.sym);
         writeln (output, '  : ', goscan^.go^.statenum);
         goscan := goscan^.next;
      end; {while} 

      writeln (output, '==========================');

      scan := scan^.next;
   end; {while} 
   
   writeln (output, '=====================');

end; {wrmac}

{------------------------------------------------------------------
 ------------------------------------------------------------------} 

{   error (i)
      write the error message corresponding to i and set errorfree
(global variable) to false.
}

procedure error (i : integer);

begin
   case i of
1:   writeln (output, 'too many symbols.  controlled by constant maxst.');

2:   writeln (output, 'missing keyword options.');
3:   writeln (output, 'unrecognized option.');
4:   writeln (output, 'end of file encountered while reading options.');

5:   writeln (output, 'missing keyword terminals.');
6:   writeln (output, 'end of file encountered while reading terminals.');

7:   writeln (output, 'missing keyword nonterminals.');
8:   writeln (output, 'end of file encountered while reading nonterminals.');

9:   writeln (output, 'missing keyword productions.');

25:  writeln (output, 'too many productions.', 
                        '  controlled by constant maxprods.');

30:  writeln (output, 'symbol used on left-hand side ',
                        'in neither terminal nor nonterminal lists.');
31:  writeln (output, 'symbol used on right-hand side ',
                        'in neither terminal nor nonterminal lists.');

40:  writeln (output, 'symbol too large to be read in.  ', 
               'controlled by constant maxwdlen.');

50:  writeln (output, 'terminal symbol used on left-hand side.');
51:  writeln (output, 'terminal symbol never used on right-hand side.');
52:  writeln (output, 'nonterminal symbol never used on left-hand side.');
53:  writeln (output, 'nonterminal symbol never used on right-hand side.');

60:  writeln (output, 'production used out of order;  all with same ',
               'left-hand side must be grouped together.');

70:  writeln (output, 'no options are in effect.');

   end;  {case}

   errorfree := false;

end; {error}

{------------------------------------------------------------------
 ------------------------------------------------------------------} 

   {input/output procedures and functions:  initio, writeword, strequal,
shift, and readword.}

{   initio
initialize keywords, option words, and the input file.
}

procedure initio;
begin
   ch := ' ';
   while (ch <= ' ') and not eof (input) do
   begin
      read (input, ch);
   end;

   optnwd.length := 7;
   with optnwd do
   begin
      value [1] := 'o';  value [2] := 'p';  value [3] := 't';
      value [4] := 'i';  value [5] := 'o';  value [6] := 'n';
      value [7] := 's';
   end;

   termwd.length := 9;
   with termwd do
   begin
      value [1] := 't';  value [2] := 'e';  value [3] := 'r';
      value [4] := 'm';  value [5] := 'i';  value [6] := 'n';
      value [7] := 'a';  value [8] := 'l';  value [9] := 's';
   end;

   nontermwd.length := 12;
   with nontermwd do
   begin
      value [1] := 'n';  value [2] := 'o';  value [3] := 'n';
      value [4] := 't';  value [5] := 'e';  value [6] := 'r';
      value [7] := 'm';  value [8] := 'i';  value [9] := 'n';
      value [10] := 'a';  value [11] := 'l';  value [12] := 's';
   end;

   prodnwd.length := 11;
   with prodnwd do
   begin
      value [1] := 'p';  value [2] := 'r';  value [3] := 'o';
      value [4] := 'd';  value [5] := 'u';  value [6] := 'c';
      value [7] := 't';  value [8] := 'i';  value [9] := 'o';
      value [10] := 'n';  value [11] := 's';  
   end;

   fsmwd.length := 3;
   with fsmwd do
   begin
      value [1] := 'f';  value [2] := 's';  value [3] := 'm';
   end;

   minwd.length := 3;
   with minwd do
   begin
      value [1] := 'm';  value [2] := 'i';  value [3] := 'n';
   end;

   slrwd.length := 3;
   with slrwd do
   begin
      value [1] := 's';  value [2] := 'l';  value [3] := 'r';
   end;

   lalrwd.length := 4;
   with lalrwd do
   begin
      value [1] := 'l';  value [2] := 'a';  value [3] := 'l';
      value [4] := 'r';  
   end;

   fstfolwd.length := 6;
   with fstfolwd do
   begin
      value [1] := 'f';  value [2] := 's';  value [3] := 't';
      value [4] := 'f';  value [5] := 'o';  value [6] := 'l';
   end;

   lrwd.length := 2;
   with lrwd do
   begin
      value [1] := 'l';  value [2] := 'r';
   end;


   tblwd.length := 5;
   with tblwd do 
   begin
      value [1] := 't';  value [2] := 'a';  value [3] := 'b';
      value [4] := 'l';  value [5] := 'e';  
   end;


   echowd.length := 4;
   with echowd do 
   begin
      value [1] := 'e';  value [2] := 'c';  value [3] := 'h';
      value [4] := 'o';  
   end;

   eopwd.length := 1;
   eopwd.value [1] := '&';

   eofwd.length := 0;

   dolwd.length := 1;
   dolwd.value [1] := '$';

   sprimewd.length := 2;
   sprimewd.value [1] := 's';   sprimewd.value [2] := '''';

end;

{   strequal (w1, w2)
true if w1 and w2 are equivalent strings, else false.
}

function strequal (w1, w2 : wordtype) : boolean;
   var match : boolean;
       i : integer;
begin
   if w1.length <> w2.length then
      strequal := false
   else
   begin
      i := 0;
      match := true;

      while (i < w1.length) and match do
      begin
         i := i + 1;
         match := w1.value [i] = w2.value [i];
      end;

      strequal := match;
   end;
end;  { strequal }

{   writeword (f, w)
write the word w to the file f.
}

procedure writeword (var f : text; w : wordtype);
   var
      i : integer;
begin
   for i := 1 to w.length do
      write (f, w.value [i]);
end;  (* writeword *)

{   shift (w)
shift the word w to uppercase.  (used for comparison to the keywords and
options.)
}

procedure shift (var wt : wordtype; w : wordtype);
   var w2 : wordtype;
       i : integer;
begin

   w2.length := w.length;

   for i := 1 to w.length do
   begin
      w2.value [i] := w.value [i];

      if (w.value [i] >= 'a') and (w.value [i] <= 'z') then
         w2.value [i] := chr (ord (w.value [i]) - ord ('a') + ord ('a'));
   end;

   wt := w2;

end; {shift}

{   readword (w)
read the word w from the input file.
}

procedure readword (var w : wordtype);
   var i : integer;
begin
   if eof (input) then
      w.length := 0
   else
   begin
      i := 0;
      while (ch > ' ') and not eof (input) and (i < maxwdlen) do
      begin
         i := i + 1;
	 w.value [i] := ch;
         read (input, ch);
      end;

      if (i >= maxwdlen) and ( (ch > ' ') or eof (input)) then
         error (40 {word input too long});

      w.length := i;

      while (ch <= ' ') and not eof (input) do
         read (input, ch);

   end;
end;  { readword }

{------------------------------------------------------------------
 ------------------------------------------------------------------} 

{      symbol table routines:  initst, find, lookupl, lookupr, lookupn, isnont}


{   initst
initialize the symbol table.
}

procedure initst;
   var i : integer;
begin
   curst := 0;

   for i := 0 to maxst do 
   begin 
      symtab [i].usedlhs := false;
      symtab [i].usedrhs := false;
      symtab [i].usedprod := false;
   end; {for} 

end;  {initst}

{   insertst (inword, inisterm, inpos)
insert the word inword into the symbol table, with inisterm indicating
whether it is a terminal or nonterminal, and inpos indicating its
number (position in the input file).}

procedure insertst (inword : wordtype;  inisterm : boolean; inpos : integer);
begin

   if curst >= maxst then
      error (1 {'symbol table overflow'})
   else
   begin 
      curst := curst + 1;
      with symtab [curst] do
      begin
         word := inword;
         isterminal := inisterm;
         pos := inpos;
         usedlhs := false;
         usedrhs := false;
      end;
   end; 

end;  {insertst}

{   find (w)
given the word w, look for it in the current symbol table, and return
the corresponding symbol if found, else return the empty symbol.}

function find (w : wordtype) : symbol;
   var i : integer;
       found : boolean;
begin
   i := 0;
   found := false;

   while (i < curst) and not found do 
   begin 
      i := i + 1;
      found := strequal (symtab [i].word, w);
   end; {while}

   if not found then find := emptysym
   else find := i;

end; {function}

{   lookupl (w, pn)
the word w is used on a left hand side;  flag this and return the
symbol corresponding to w. if error, print pn.}

function lookupl (w : wordtype; pn : integer) : symbol;
   var t : symbol;
begin
   t := find (w);

   if t = emptysym then 
   begin 
      error (30 {symbol not found lookupl  w});
      write (output, '   occurred in production # ', pn:3, ' on symbol ');
      writeword (output, w);
      writeln (output);
      writeln (output);
   end 
   else 
   begin 
      symtab [t].usedlhs := true;
   end; {if} 

   lookupl := t;
end; {lookupl}

{   lookupr (w, pn)
the word w is used on a right-hand side;  record this and return
the corresponding symbol.  if error, print pn.}

function lookupr (w : wordtype; pn : integer) : symbol;
   var t : symbol;
begin
   t := find (w);

   if t = emptysym then 
   begin 
      error (31 {symbol not found in lookupr  w});
      write (output, '   occurred in production # ', pn:3, ' on symbol ');
      writeword (output, w);
      writeln (output);
      writeln (output);
   end 
   else 
   begin 
      symtab [t].usedrhs := true;
   end; {if} 

   lookupr := t;
end; {lookupr}

{   lookupn (n)
return the nth symbol in the symbol table.}

procedure lookupn (var wtsym: wordtype; n : symbol);

begin
   if (n < 1) or (n > curst) then
      wtsym := eofwd
   else
      wtsym := symtab [n].word;
end; {lookupn}





{   isnont (s)
      true if s is a nonterminal, else false.}

function isnont (s : symbol) : boolean;

begin
   isnont := (s > numt);
end; {isnont}   
   
{------------------------------------------------------------------
 ------------------------------------------------------------------} 

{      production routines:  initprod, putlhs, putrhs, lastrhs, nextprod}

{   initprod
initialize the production array.}

procedure initprod ;
begin
   curprod := 0;
   prodarray [0].lhs := 0;

end; {initprod}

{   putlhs (s, pn)
establish symbol s as the lhs of production # pn.}

procedure putlhs (s : symbol; pn : integer) ;

begin
   with prodarray [curprod] do
   begin
      num := pn;
      lhs := s;
      
      new (rhlist);  {dummy node}
      rhlist^.sym := 0;
      rhlist^.next := nil;
      rhend := rhlist;
   end;

   if prodarray [curprod].lhs <> prodarray [curprod - 1].lhs then 
   begin 
      prodindex [s].index := curprod;
      prodindex [s].count := 1;
   end
   else
   begin
      prodindex [s].count := prodindex [s].count + 1;
   end;

end; {putlhs}

{   putrhs (s)
establish s as having occurred in the current production}

procedure putrhs (s : symbol);

begin
   with prodarray [curprod] do
   begin 
      new (rhend^.next);
      rhend := rhend^.next;
      rhend^.sym := s;
      rhend^.next := nil;
   end; 
   
end; {putrhs}

{   lastrhs
close off the rhs of the current production.}

procedure lastrhs;
begin 
   prodarray [curprod].rhlist := prodarray [curprod].rhlist^.next;
         {remove dummy}
end; {lastrhs}




{   nextprod
move to the next production}

procedure nextprod ;

begin
   if curprod >= maxprods then 
   begin 
      error (25 {'too many productions'});
   end 
   else 
   begin 
      curprod := curprod + 1;
   end; {if} 
end; {nextprod}

{------------------------------------------------------------------
 ------------------------------------------------------------------} 
   
{      production reference routines:  lhsn, rhsn, donerhs, rhssym, nextrhs}



{   lhsn (pn)
return the left-hand side corresponding to production # pn.}

function lhsn (pn : integer) : symbol;
begin
   lhsn := prodarray [pn].lhs;
end; {lhsn}



{   rhsn (pn)
return the right-hand side corresponding to prod. # pn.}

function rhsn (pn : integer) : prodlist;
begin
   rhsn := prodarray [pn].rhlist;
end; {rhsn}




{   donerhs (t)
true if there is nothing left to look at in the current right-hand side.}

function donerhs (t : prodlist) : boolean;
begin
   donerhs := t = nil;
end; {function}

{   rhssym (t)
return the symbol corresponding to the current position in t.}

function rhssym (t : prodlist) : symbol;

begin
   rhssym := t^.sym;
end; {function}




{   nextrhs (t)
move one to the right in the rhs t.}

function nextrhs (t : prodlist) : prodlist;

begin
   nextrhs := t^.next;
end; {function}

{-----------------------------------------------------------------
 -----------------------------------------------------------------}

{      first and follow routines:  printff, addfirst, addprodfirst,
createfirst, firststr, first2, addfol, addprodfol, createfollow.}


{   printff
print the first and follow sets for all nonterminals.}

procedure printff;
   var i : integer;
      sym : symbol;
begin
   writeln (output, '     ');
   writeln (output, '     ');
   
   writeln (output, '   first and follow sets for the nonterminals');
   writeln (output);

   for i := numt + 1 to numt + numnt do 
   begin 
      intnotused := writesym (i);
      writeln (output);

      writeln (output, 'first:  ');
      writeset (symtab [i].first, ' ');
      writeln (output);

      writeln (output, 'follow:  ');
      writeset (symtab [i].follow, ' ');      
      writeln (output);

      writeln (output);
      writeln (output);
   end; {for} 

end; {printff}

{   addfirst (sym, s)
add s to the first set for sym.  return true if any change resulted.}

function addfirst (sym : symbol;  s : symset) : boolean;
   var change : boolean;
begin
   change := not (s <= symtab [sym].first);
   
   if change then 
   begin 
      symtab [sym].first := symtab [sym].first + s;
   end; {if} 

   addfirst := change;
end; {addfirst}

{   addprodfirst (x, s)
the list s represents a rhs.  add sets to the first set of x depending
on the first set of the items in s.}

function addprodfirst (x : symbol; s : prodlist) : boolean;
   var scan : prodlist;
       added, moreempty : boolean;
       sset : symset;
begin
   scan := s;
   added := false;
   
   repeat
      sset := symtab [scan^.sym].first;
      added := added or addfirst (x, sset - [emptysym]);
      moreempty := emptysym in sset;
      scan := scan^.next;
   until (scan = nil) or not moreempty;

   if (scan = nil) and moreempty then 
   begin 
      added := added or addfirst (x, [emptysym]);
   end; {if} 

   addprodfirst := added;
end; {addprodfirst}

{   createfirst
build the first set for all symbols.}

procedure createfirst;
   var i : integer;
       change : boolean;
       lhs : symbol;
begin
   for i := emptysym to numt do 
   begin 
      symtab [i].first := [i];
   end; {for} 

   for i := numt + 1 to numt + numnt do 
   begin 
      symtab [i].first := [];
   end; {for} 

   repeat
      change := false;
      
      for i := 1 to nump do 
      begin 
	 lhs := prodarray [i].lhs;

         if prodarray [i].rhlist = nil then 
	 begin 
	    change := change or addfirst (lhs, [emptysym]);
	 end 
	 else if not isnont (prodarray [i].rhlist^.sym) then 
	 begin 
	    change := change or addfirst (lhs, [prodarray [i].rhlist^.sym]);
	 end 
	 else 
	 begin 
	    change := change or addprodfirst (lhs, prodarray [i].rhlist);
	 end; {if} 

      end; {for} 

   until not change;
end; {createfirst}

{   firststr (s)
return the first of the list of symbols s.}

procedure firststr (var xxsymset:symset; s : prodlist); 
   var scan : prodlist;
       oldset, newset : symset;
begin
   scan := s;
   oldset := [];
   newset := [emptysym];

   while (scan <> nil) and (emptysym in newset) do 
   begin 
      newset := symtab [scan^.sym].first;
      oldset := oldset + newset - [emptysym];
      scan := scan^.next;
   end; {while} 

   if (scan = nil) and (emptysym in newset) then 
   begin 
      oldset := oldset + [emptysym];
   end; {if} 

   xxsymset := oldset;

end; {firststr}

{   first2 (l, sym)
return the first set of l concatenated with each element of sym.}

procedure first2 (var yysymset:symset; l : prodlist; sym : symset);
   var tempset : symset;
begin
   firststr (tempset, l);

   if emptysym in tempset then
      tempset := tempset + sym - [emptysym];

   yysymset := tempset;

end; {first2}

{-----}
{   addfol (x,s)
add the set s to the follow set of x.  return true if any change was made.}

function addfol (x : symbol;  s : symset) : boolean;
   var change : boolean;
begin
   change := not (s <= symtab [x].follow);

   if change then
      symtab [x].follow := symtab [x].follow + s;

   addfol := change;

end; {addfol}

{   addprodfol (lhs, s)
while each symbol of s has the empty symbol in its follow set,
add it (set) to the follow set of lhs.}

function addprodfol (lhs : symbol; s : prodlist) : boolean;
   var scan : prodlist;
       added : boolean;
       addset : symset;
begin
   scan := s;
   added := false;

   repeat
      if isnont (scan^.sym) then 
      begin 
	 firststr (addset, scan^.next);
         
         if emptysym in addset then
            addset := addset + symtab [lhs].follow - [emptysym];

         added := added or addfol (scan^.sym, addset);
      end; {if} 

      scan := scan^.next;
   until scan = nil;

   addprodfol := added;
end; {addprodfol}

{   createfollow
create the follow sets for all the nonterminals.}

procedure createfollow;
   var change : boolean;
       i : integer;
       lhs : symbol;
begin
   boolnotused := addfol (startlhssym, [dolsym]);

   repeat
      change := false;

      for i := 1 to nump do 
      begin 
         lhs := prodarray [i].lhs;
         if prodarray [i].rhlist <> nil then 
         begin 
            change := change or addprodfol (lhs, prodarray [i].rhlist);
         end; {if} 
      end; {for} 

   until not change;

end; {createfollow}

{------------------------------------------------------------------}

{      item routines:  itemtosymbol, makeitem}


{   itemtosymbol (i)
given an item, return the symbol corresponding to its designated position.}

function itemtosymbol (i : item1) : symbol;

begin
   if i.dp = nil then 
   begin 
      itemtosymbol := emptysym;
   end 
   else 
   begin 
      itemtosymbol := i.dp ^.sym;
   end; {if} 
end; {itemtosymbol}


{   makeitem (i, sym)
given a production i, convert it into a new item with lookahead set sym.}

procedure makeitem (var xxitem1:item1; i : production; sym : symset);
   var result : item1;
begin
   result.prod := i;
   result.dp := prodarray [i].rhlist;
   result.sym := sym;
   xxitem1 := result;
end; {makeitem}

{------------------------------------------------------------------}

{      closure routines:  initcl, donecl, curclitem, curclsym, curclrest,
nextcl, memcl, addcl0, addcl1, startlhs, donelhs, nextlhs, closure0,
closure1.}


{   initcl (i, startcl, curcl, endcl)
create a new set of items containing only i, with starting and ending pointers,
and the current pointer pointing to a preceding dummy node.}

procedure initcl (i : item1set;  var startcl, curcl, endcl : item1set);
   var scan : item1set;   
       temp : item1set;
begin
   {make copy of i in startcl}
   new (endcl);
   endcl^.item := i^.item;
   endcl^.next := nil;

   startcl := endcl;
   scan := i^.next;
   
   while scan <> nil do 
   begin 
      new (temp);
      temp^.item := scan^.item;
      temp^.next := startcl;
      startcl := temp;
      scan := scan^.next;
   end; {while}

   curcl := startcl;

end; {initcl}

{   donecl (curcl)
true if the current closure is empty (ie. the whole list has been scanned).}

function donecl (curcl : item1set) : boolean;
begin
   donecl := (curcl = nil);
end; {donecl}



{   curclitem (curcl)
return the item corresponding to the current position in the set of items.}

{
function curclitem (curcl : item1set) : item1;
begin
   curclitem := curcl^.item;
end; } {curclitem}


{   curclsym (curcl)
return the lookahead set corresponding to the current pos. in the item set.}

{
function curclsym (curcl : item1set) : symset;
begin
   curclsym := curcl^.item.sym;
end; } {curclsym}


{   curclrest (curcl)
return the next rest of the production from the current item set (moving
the distinguished position forward.}

function curclrest (curcl : item1set) : prodlist;
begin
   curclrest := curcl^.item.dp^.next;
end; {curclrest}


{   nextcl (curcl)
return the next item in the current list.}
function nextcl (curcl : item1set) : item1set;
begin
   nextcl := curcl ^.next;
end; {nextcl}

{   memcl (i, startcl, lr0)
return true if the item i matches any item in the set startcl.  compare
lookaheads only if lr0 is false.}

function memcl (i : item1; startcl : item1set; lr0 : boolean) : boolean;
   var flag : boolean;
       scan : item1set;
begin
   flag := false;
   scan := startcl;
   
   while (scan <> nil) and not flag do 
   begin 
      flag := (scan ^.item.prod = i.prod) and (scan^.item.dp = i.dp)
         and (lr0 or (scan^.item.sym = i.sym));
      scan := scan^.next;
   end; {while}
   
   memcl := flag;

end; {memcl}

{   addcl0 (i, endcl)
add the lr(0) item i to the end of the list of items.}

procedure addcl0 (i : item1;  var endcl : item1set);
   var temp : item1set;
begin

   new (temp);
   temp^.item := i;
   temp^.next := nil;

   endcl ^.next := temp;
   endcl := temp;
end; {addcl}



{   addcl1 (i, startcl, endcl)
add the item i to the list startcl;  modify the end position.  note that
this routine (vs. addcl0) must check that no item exists differing
only in its lookahead.}

procedure addcl1 (i : item1;  startcl : item1set; var endcl : item1set);
   var flag : boolean;
       last, scan : item1set;
       temp : item1set;
begin
   flag := false;
   scan := startcl;
   
   while (scan <> nil) and not flag do 
   begin 
      last := scan;
      flag := (scan ^.item.prod = i.prod) and (scan^.item.dp = i.dp);
      scan := scan^.next;
   end; {while}
   
   if flag then
      last^.item.sym := last^.item.sym + i.sym
   else
   begin
      new (temp);
      temp^.item := i;
      temp^.next := nil;

      endcl ^.next := temp;
      endcl := temp;
   end; {if}

end; {addcl1}

{   startlhs (t, lhsstart, lhscur, lhsmax)
set up the starting, current, and maximum production indexes
according to the lhs symbol t.}

procedure startlhs (t : symbol;  var lhsstart, lhscur, lhsmax : production);

begin
   lhsstart := prodindex [t].index;
   lhscur := lhsstart;
   lhsmax := lhsstart + prodindex [t].count;
end; {startlhs}



{   donelhs (lhscur, lhsmax)
true if the current production is past the last to be processed.}

function donelhs (lhscur, lhsmax : production) : boolean;
begin
   donelhs := lhscur >= lhsmax;
end; {donelhs}



{   nextlhs (lhscur)
given the current production, move to the next to be processed.}

function nextlhs (lhscur : production) : production;
begin
   nextlhs := lhscur + 1;
end; {nextlhs}

{   closure0 (i)
return the lr(0) closure of the set of items i.}

function closure0 (i : item1set) : item1set;
   var t : symbol;
       itm : item1;

       startcl, curcl, endcl : item1set;

       lhsstart, lhsmax, lhscur : production;

begin      {closure0}
   initcl (i, startcl, curcl, endcl);

   while not donecl (curcl) do 
   begin 
      t := itemtosymbol (curcl^.item);
      if isnont (t) then 
      begin 
	 startlhs (t, lhsstart, lhscur, lhsmax);
         while not donelhs (lhscur, lhsmax) do 
	 begin 
	    makeitem (itm, lhscur, []);
            if not memcl(itm, startcl, true {lr0}) then 
	    begin 
	       addcl0 (itm, endcl);
	    end; {if} 

            lhscur := nextlhs (lhscur);
	 end; {while}
      end; {if} 
      
      curcl := nextcl (curcl);
   end; {while}

   closure0 := startcl;
end; {closure0}

{   closure1 (i)
return the lr(1) closure corresponding to the set of items i.}

function closure1 (i : item1set) : item1set;
   var t : symbol;
       itm : item1;
       startcl, curcl, endcl : item1set;
       lhsstart, lhsmax, lhscur : production;
       termsym : symbol;
       yysymset: symset;

begin      {closure1}
   initcl (i, startcl, curcl, endcl);

   while not donecl (curcl) do 
   begin 
      t := itemtosymbol (curcl^.item);

      if isnont (t) then 
      begin 
	 startlhs (t, lhsstart, lhscur, lhsmax);

         while not donelhs (lhscur, lhsmax) do 
	 begin 
	    first2 (yysymset, curclrest (curcl), curcl^.item.sym);
	    makeitem (itm, lhscur, yysymset);

	    if not memcl (itm, startcl, false {not lr0}) then 
	    begin 
	       addcl1 (itm, startcl, endcl);
            end; {if}

            lhscur := nextlhs (lhscur);
	 end; {while}

      end; {if} 
      
      curcl := nextcl (curcl);
   end; {while}

   closure1 := startcl;

end; {closure1}

{-------------------------------------------------------------
 -------------------------------------------------------------}

{      fsm routines:  inititmset, initiset, nomoreiset, nextiset,
initgoto, moregoto, dfirst, eqitemset, memiset, additem, recordgo,
lr0items, lr1items.}


{   inititmset
create the initial set of items for use in lr(0) and lr(1) machines.}

function inititmset : item1set;
   var temp : item1set;
begin
   new (temp);
   temp^.item.prod := sprimepn;
   temp^.item.dp := prodarray [sprimepn].rhlist;
   temp^.item.sym := [dolsym];
   temp^.next := nil;

   inititmset := temp;
end; {inititmset}

{   initiset (i, startiset, curiset, endiset)
given the set of items i, create the new machine with starting and ending
pointers, and the current pointer pointing to a preceding dummy node.}

procedure initiset (i : item1set; var startiset, curiset, endiset : lrmac);

begin
   new (curiset);      {dummy node}
   curiset^.golist := nil; { JDA }
   curiset^.lalrset := nil; { JDA }
   new (curiset^.next);
   
   startiset := curiset ^.next;
   endiset :=  startiset;

   startiset^.itemset := i;
   startiset^.statenum := 1;
   startiset^.golist := nil;
   startiset^.lalrset := nil; { JDA }
   startiset^.next := nil;

end; {initiset}

{   nomoreiset (curiset, endiset)
true if curiset has reached the end of the machine (endiset).}

function nomoreiset (curiset, endiset : lrmac) : boolean;
begin
   nomoreiset := (curiset = endiset);
end; {nomoreiset}



{   nextiset (curiset)
move to the next set of items (state) in the list.}

function nextiset (var curiset : lrmac) : item1set;
begin
   curiset := curiset ^.next;
   nextiset := curiset ^.itemset;
end; {nextiset}

{   initgoto (i, curgoto)
given the set i, return in curgoto the nodes which have an lr goto.}

procedure initgoto (i : item1set; var curgoto : item1set);
   var scan, temp : item1set;
begin
   {copy from i to curgoto only those nodes with a non-nil distinguished
      part.  these are the nodes which may have an lr goto.}
   curgoto := nil;
   scan := i;

   while scan <> nil do 
   begin 
      if scan^.item.dp <> nil then 
      begin 
	 new (temp);
	 temp^.item := scan^.item;
	 temp^.next := curgoto;
	 curgoto := temp;
      end; {if} 

      scan := scan^.next;
   end; {while}
end; {initgoto}



{   moregoto (curgoto)
true if there are more unchecked nodes for an lr goto.}

function moregoto (curgoto : item1set) : boolean;
begin
   moregoto := (curgoto <> nil);
end; {moregoto}

{   dfirst (curgoto, gotosym)
remove from curgoto all items corresponding to gotosym, the first symbol
on the curgoto list.}

function dfirst (var curgoto : item1set;  var gotosym : symbol) : item1set;
   var 
       temp, list : item1set;
       scan, scan2 : item1set;
begin
   {move from curgoto to list all those items with symbols identical
      to that of the first item on curgoto, having moved over them.
      set curgoto to the list of remaining items.  return list as the 
      list of states to be closed.}

   gotosym := itemtosymbol (curgoto^.item);

   list := nil;
   scan2 := curgoto;
   scan := scan2^.next;
   
   while scan <> nil do 
   begin 
      if itemtosymbol (scan^.item) = gotosym then 
      begin 
         scan2^.next := scan^.next;
         scan^.next := list;
         scan^.item.dp := scan^.item.dp^.next;
         list := scan;
         scan := scan2^.next;
      end
      else
      begin
         scan2 := scan;
         scan := scan^.next;
      end; {if}
   end; {while}

   {move first item on curgoto to list}
   temp := curgoto;
   curgoto := curgoto ^.next;
   temp ^.item.dp := temp^.item.dp^.next;
   temp ^.next := list;

   dfirst := temp;
end; {dfirst}

{   eqitemset (i1, i2, lr0)
return true if the sets of items i1 and i2 are equivalent.  check lookaheads
only if lr0 is false.}

function eqitemset (i1, i2 : item1set; lr0 : boolean) : boolean;
   var s1, s2 : item1set;
       match, found : boolean;
begin
   s1 := i1;  s2 := i2;
   while (s1 <> nil) and (s2 <> nil) do 
   begin 
      s1 := s1^.next;
      s2 := s2^.next;
   end; {while} 
   match := (s1 = s2);  {both nil together}

   s1 := i1;
   while (s1 <> nil) and match do 
   begin 
      s2 := i2;
      found := false;
      while (s2 <> nil) and not found do 
      begin 
	 found := (s1^.item.prod = s2^.item.prod) 
            and   (s1^.item.dp = s2^.item.dp)
            and   (lr0 or (s1^.item.sym = s2^.item.sym));
         s2 := s2^.next;
      end; {while} 
      match := match and found;
      s1 := s1^.next;
   end; {while} 

   eqitemset := match;
end; {eqitemset}

{   memiset (goset, goiset, startiset, lr0)
check the list startiset for an item equivalent to goset (checking lookaheads
only if lr0 is false).  save that item as goiset.}

function memiset (goset : item1set; var goiset : lrmac;
            startiset : lrmac; lr0 : boolean) 
            : boolean;
   var scan : lrmac;
       found : boolean;
begin
   scan := startiset;
   found := false;

   while (scan <> nil) and not found do 
   begin 
      goiset := scan;
      found := eqitemset (goset, scan^.itemset, lr0);
      scan := scan^.next;
   end; {while} 
   
   memiset := found;
end; {memiset}

{   additem (s, i, goiset, endiset)
add the set i to the end of the machine (endiset), saving this as the
set gone to.}

procedure additem (i : item1set; var goiset, endiset : lrmac);
begin
   new (endiset^.next);
   endiset^.next^.statenum := endiset^.statenum + 1;
   endiset^.next^.itemset := i;
   endiset^.next^.golist := nil;
   endiset^.next^.lalrset := nil; { JDA }
   endiset^.next^.next := nil; { JDA }
   endiset := endiset^.next;
   goiset := endiset;
end; {additem}




{   recordgo (s, goiset, curiset)
note that from curiset, one transfers to goiset on symbol s.}

procedure recordgo (s : symbol; goiset, curiset : lrmac);
   var tempgo : goptr;
begin

   new (tempgo);
   tempgo^.sym := s;
   tempgo^.go := goiset;
   tempgo^.next := curiset^.golist;
   curiset^.golist := tempgo;

end; {recordgo}

{-------------------------------------------------------------------}

{   lr0items
calculate the set of lr(0) items.}

function lr0items : lrmac;
   var goset : item1set;

   startiset, curiset, endiset, goiset : lrmac;

   curgoto : item1set;

   gotosym : symbol;      {last symbol moved over in goto}

begin      {lr0items}

   initiset (closure0 (inititmset), startiset, curiset, endiset);

   repeat
      initgoto (nextiset (curiset), curgoto);

      while moregoto (curgoto) do 
      begin 
         goset := closure0 (dfirst (curgoto, gotosym));

         if not memiset (goset, goiset, startiset, true {lr0}) then 
            additem (goset, goiset, endiset);

         recordgo (gotosym, goiset, curiset);

      end; {while}

   until nomoreiset (curiset, endiset);

   lr0items := startiset;

end; {lr0items}
{---------------------------------------------------------------------
----------------------------------------------------------------------}

{-------------------------------------------------------------------}

{   lr1items
calculate the set of lr(1) items.}

function lr1items : lrmac;
   var 
      goset : item1set;
      curgoto : item1set;

      startiset, curiset, endiset, goiset : lrmac;
      gotosym : symbol;      {last symbol moved over in goto}

begin   {lr1items}

   initiset (closure1 (inititmset), startiset, curiset, endiset);

   repeat
      initgoto (nextiset (curiset), curgoto);

      while moregoto (curgoto) do 
      begin 
         goset := closure1 (dfirst (curgoto, gotosym));

         if not memiset (goset, goiset, startiset, false {not lr0}) then 
	 begin 
            additem (goset, goiset, endiset);
	 end; {if} 

         recordgo (gotosym, goiset, curiset);
      end; {while}

   until nomoreiset (curiset, endiset);

   lr1items := startiset;

end; {lr1items}

{---------------------------------------------------------------------
----------------------------------------------------------------------}

{      lr machine check routines:  checklr, count, writetype, printheader1,
errors, shifts, reduceslr, reducelr1, printrow, printheader2, initgo,
gotos, printgo, printconflicts.}


{   checklr (m, lrkind)
check the machine m for being of the lr type specified by lrkind.}

function checklr (m : lrmac;  lrkind : lr1type) : boolean;


   const
      shift = 1;
      reduce = 2;
      error = 3;
      accept = 4;

   type
      conptr = ^conrec;

      conrec = record
	 state : integer;
	 ctype : 1..4;
	 cnum : integer;
	 sym : symbol;
	 next : conptr;
      end;


      actrowtype = array [symbol] of record
	 act : 1..4;
	 num : integer;
      end;

      gorowtype = array [symbol] of integer;

   var 
      mscan : lrmac;
      actrow : actrowtype;      
      gorow : gorowtype;

      conflicts : conptr;
      size : integer;

{   count (m)
return the number of states in the machine m.}

function count (m : lrmac) : integer;
   var mscan : lrmac;
       size : integer;
begin
   mscan := m;
   size := 0;
   
   while mscan <> nil do 
   begin 
      size := size + 1;
      mscan := mscan^.next;
   end; {while} 

   count := size;
end; {count}

{   writetype (lrkind)
write slr, lalr, or lr depending on lrkind.}

procedure writetype (lrkind : lr1type);

begin
      case lrkind of 
slrtype   :   write (output, 'slr');
lalrtype  :   write (output, 'lalr');
lrtype    :   write (output, 'lr');
      end;   
end; {writetype}


{   printlabels (lo,hi)
print the labels on top of the tables from symbols lo to hi. }

procedure printlabels (lo, hi : symbol); 
   var i : integer;
      cols : integer;
      size : integer;
begin
   write (output , '{      ');
   cols := 1;   
   for i := lo to hi do
   begin
      write (output, ' ');
      size := writesymsh (i, 7);

      cols := cols + 8;
      if cols > maxpagecols then 
      begin
         writeln (output);
         cols := 2;
      end;

   end; {for}
   writeln (output);
   writeln (output, '}');

end; {printlabels}

{   printheader1 (lrkind, size)
print the heading for the action table.}

procedure printheader1 (lrkind : lr1type;  size : integer);
begin
   writeln (output, '     ');
      
   writeln (output, '     ');
   writetype (lrkind);
   writeln (output, ' (1)  parse table');  

   writeln (output);
   writeln (output, 'this table consists of the action part ',
                    'of the parse table');
   writeln (output);
   writeln (output, 'action : array [1 .. ', size:3, 
               ' , 1 .. ', numt:3, ']  of record');
   writeln (output, '   act : 1..4;');
   writeln (output, '   num : integer;');
   writeln (output, '   end;');
   writeln (output, ' ');
   writeln (output, 'action is an array of (act,num) pairs');
   writeln (output, 'it is accessed by curr_state x terminal pair');
   writeln (output, ' ');
   writeln (output, '   for the act entry:');
   writeln (output,'       1 => shift, 2 => reduce, 3 => error, 4 => accept');
   writeln (output, ' ');
   writeln (output, '   for the num entry:');
   writeln (output,'       if act indicates a shift then num represents the');
   writeln (output,'          state being moved to (i.e. the next state)');
   writeln (output,'       if act indicates a reduce then num represents ');
   writeln (output,'          the number of the production being used for ');
   writeln (output,'          the reduction');
   writeln (output,' ');


end; {printheader1}

{   errors (actrow)
set all locations in the action table as initially being errors.}

procedure errors (var actrow : actrowtype);
   var i : integer;
begin
   for i := 1 to numt do 
   begin 
      actrow [i].act := error;
      actrow [i].num := 0;
   end; {for} 

end; {errors}

{   shifts (actrow, golist)
given a golist for a state, calculate the shifts in that state.}

procedure shifts (var actrow : actrowtype;  golist : goptr);
   var goscan : goptr;
begin
   goscan := golist;
   while goscan <> nil do 
   begin 
      if not isnont (goscan^.sym) then 
      begin 
	 actrow [goscan^.sym].act := shift;
         actrow [goscan^.sym].num := goscan^.go^.statenum;
      end; {if} 

      goscan := goscan^.next;
   end; {while} 

end; {shifts}

{   reduceslr (actrow, iset, statenum, conflicts)
calculate the slr reductions to be done in actrow according to the set of items
iset in row # statenum.  if there are any conflicts, return them in the
list conflicts.}

procedure reduceslr (var actrow : actrowtype;  iset : item1set;  
      statenum : integer;  var conflicts : conptr);
   
   var iscan : item1set;
      i : integer;
      pn : production;
      sym : symbol;
      newcon : conptr;
begin
   iscan := iset;
   while iscan <> nil do 
   begin 
      if iscan^.item.dp = nil then 
      begin 
         pn := iscan^.item.prod;
	 sym := prodarray [pn].lhs;
         if sym = sprimesym then actrow [dolsym].act := accept;

         for i := 1 to numt do 
	 begin 
	    if i in symtab [sym].follow then 
	    begin 
	       if actrow [i].act <> error then 
	       begin 
		  new (newcon);
                  newcon^.state := statenum;
                  newcon^.ctype := actrow [i].act;
                  newcon^.cnum := actrow [i].num;
                  newcon^.sym := i;
                  newcon^.next := conflicts;
                  conflicts := newcon;
	       end; {if} 

	       actrow [i].act := reduce;
	       actrow [i].num := pn;

	    end; {if} 

	 end; {for} 

      end; {if} 

      iscan := iscan ^.next;

   end; {while} 

end; {reduceslr}

{   reducelr1 (actrow, iset, statenum, conflicts)
calculate the lr(1) reductions to be done in actrow according to the set 
of items iset in row # statenum.  if there are any conflicts, return them 
in the list conflicts.  note that this routine is also used for lalr(1)
reductions.}

procedure reducelr1 (var actrow : actrowtype;  iset : item1set;  
      statenum : integer;  var conflicts : conptr);
   
   var iscan : item1set;
      i : integer;
      pn : production;
      sym : symbol;
      newcon : conptr;
begin
   iscan := iset;
   while iscan <> nil do 
   begin 
      if iscan^.item.dp = nil then 
      begin 
         pn := iscan^.item.prod;

         if prodarray [pn].lhs = sprimesym then 
            actrow [dolsym].act := accept
         else
         begin 
            for sym := 1 to numt do 
            if sym in iscan^.item.sym then
            begin
	       if actrow [sym].act <> error then 
	       begin 
		  new (newcon);
		  newcon^.state := statenum;
		  newcon^.ctype := actrow [sym].act;
		  newcon^.cnum := actrow [sym].num;
		  newcon^.sym := sym;
		  newcon^.next := conflicts;
		  conflicts := newcon;
	       end;

	       actrow [sym].act := reduce;
	       actrow [sym].num := pn;
            end;  {if, for}

	 end; 


      end; {if} 

      iscan := iscan ^.next;

   end; {while} 

end; {reducelr1}

{   printrow (actrow)
print the row actrow of the action table.}

procedure printrow (actrow : actrowtype; rownum:integer);
   var i : integer;
begin

   for i := 1 to numt do
   begin

      writeln(output, 'action [',rownum:3,',',i:3,'].act := ',
                    actrow[i].act:1,';');
      writeln(output, 'action [',rownum:3,',',i:3,'].num := ',
                    actrow[i].num:3,';');

   end; {for}
end; {printrow}

{   printheader2 (lrkind, size)
print the header for the goto table.}

procedure printheader2 (lrkind : lr1type; size : integer);
begin
   writeln (output, '     ');
      
   writeln (output, '     ');
   writetype (lrkind);
   writeln (output, ' (1)  parse table');  
   writeln (output);
   writeln (output, 'this table consists of the goto part of the ',
                    'parse table');
   writeln (output);
   
   writeln (output, 'gotox : array [ 1 .. ', size:3, ' , ',
               (numt + 1):3, '..', (numt + numnt - 1):3, 
         ']  of integer');
   writeln(output);
   writeln(output,'gotox is an array of integers');
   writeln(output,'it is accessed by uncovered_state x non_terminal pairs');
   writeln(output);
   writeln(output,'    each array value represents the next state to go to,');
   writeln(output,'    a value of a -1 indicates an error.');
   writeln(output);
end; {printheader2}

{   initgo (gorow)
set all gotos as being initially errors (-1). }

procedure initgo (var gorow : gorowtype);
   var i : integer;
begin
   for i := numt + 1 to numt + numnt do 
   begin 
      gorow [i] := -1;
   end; {for} 

end; {initgo}

{   gotos (gorow, golist)
given the list golist, fill in the goto rows in gorow.}

procedure gotos (var gorow : gorowtype;  golist : goptr);
   var goscan : goptr;
begin
   goscan := golist;
   while goscan <> nil do 
   begin 
      if isnont (goscan^.sym) then 
      begin 
	 gorow [goscan^.sym] := goscan^.go^.statenum;
      end; {if} 

      goscan := goscan^.next;
   end; {while} 

end; {gotos}

{   printgo (gorow)
print the row gorow of the goto table.}

procedure printgo (gorow : gorowtype; rownum:integer);
   var i : integer;
begin

   for i := numt + 1 to numt + numnt - 1 do
   begin
      writeln(output,'gotox[',rownum:3,',',i:3,'] := ',gorow[i]:3,';');
   end; {for}

end; {printgo}

{   printconflicts (conflicts, lrkind)
print any conflicts that have occurred in filling out the table.}

function printconflicts (conflicts : conptr; lrkind : lr1type) : boolean;
   var conscan : conptr;
begin
   
   printconflicts := (conflicts = nil);

   writeln (output, '     ');
   writeln (output);

   if conflicts = nil then 
   begin 
      write (output, '   no conflicts.  the grammar is ');
      writetype (lrkind);
      writeln (output, ' (1).');
      writeln (output);
   end 
   else 
   begin    
      write (output, '   conflicts exist.  the grammar is not ');
      writetype (lrkind);
      writeln (output, ' (1).');
      writeln (output);

      conscan := conflicts;
      while conscan <> nil do 
      begin 
         if conscan^.ctype = shift then
            write (output, '   shift')
         else
            write (output, '   reduce');

	 write (output, '/reduce conflict in state ', conscan^.state:3, 
	       ' on the symbol ');
	 intnotused := writesym (conscan^.sym);
         writeln (output, '  (with former entry  (', conscan^.ctype:1, ',', 
                  conscan^.cnum:3, ')  )' );
	 writeln (output);
	 writeln (output);

         conscan := conscan^.next;
      end; {while} 

   end; {if} 

   writeln (output);

end; {printconflicts}

begin  {checklr}
   conflicts := nil;
   size := count (m);

   if tblopt in opts then
   begin
      printheader1 (lrkind, size);
   end;

   mscan := m;
   writeln(output,' ');
   while mscan <> nil do 
   begin 
      errors (actrow);
      shifts (actrow, mscan^.golist);

      if lrkind = slrtype then
         reduceslr (actrow, mscan^.itemset, mscan^.statenum, conflicts)
      else
         reducelr1 (actrow, mscan^.itemset, mscan^.statenum, conflicts);

      if tblopt in opts then
      begin
         printrow (actrow, mscan^.statenum);
      end;

      mscan := mscan^.next;
   end; {while} 


   if tblopt in opts then
   begin
      printheader2 (lrkind, size);
   end;

   mscan := m;
   writeln(output,' ');
   while mscan <> nil do 
   begin 
      initgo (gorow);
      gotos (gorow, mscan^.golist);

      if tblopt in opts then
      begin
         printgo (gorow,mscan^.statenum)
      end;

      mscan := mscan^.next;
   end; {while} 


   checklr := printconflicts (conflicts, lrkind);

end; {checklr}


{----------------------------------------------------------------------
 ----------------------------------------------------------------------}

{      lalr machine routines:  lalrmac, initinsert, copygo, union, 
copy, insert, resolve.}


{   lalrmac (m)
given an lr(1) machine, reduce it to an lalr(1) machine.}

function lalrmac (m : lrmac) : lrmac;
   var scan : lrmac;
      startlalr, endlalr : lrmac;



{   initinsert (startlalr, endlalr)
create the starting and ending pointers to the new machine, starting with
a dummy node.}
procedure initinsert (var startlalr, endlalr : lrmac);
begin
   new (startlalr);
   startlalr^.golist := nil; { JDA }
   startlalr^.lalrset := nil; { JDA }
   startlalr^.next := nil; { JDA }
   endlalr := startlalr;
   startlalr ^.statenum := 0;
end; {initinsert}

{   copygo (g)
given a goto pointer g, create a copied list of g.  this list points to
the old machine, but is later revised to refer only to the new.}

function copygo (g : goptr) : goptr;
   var goscan, firstgo, lastgo, tempgo : goptr;
begin
   goscan := g;
   new (firstgo);   {dummy node}
   firstgo^.go := nil; { JDA }
   firstgo^.next := nil; { JDA }
   lastgo := firstgo;

   while goscan <> nil do 
   begin 
      new (tempgo);
      tempgo^ := goscan^;
      lastgo^.next := tempgo;
      lastgo := tempgo;
      goscan := goscan^.next;
   end; {while} 

   lastgo ^.next := nil;

   copygo := firstgo^.next;
end; {copygo}

{   union (i1, i2)
given two sets of items i1 and i2, calculate their lalr(1) union.}

function union (i1, i2 : item1set) : item1set;
   var i1scan, i2scan : item1set;
       first, last : item1set;
       match : boolean;
begin
   new (first);   {create leading dummy node}
   first^.next := nil; { JDA }
   last := first;
   i1scan := i1;

   while i1scan <> nil do 
   begin 
      match := false;
      i2scan := i2;
      while (i2scan <> nil) and not match do 
      begin 
	 match := (i1scan^.item.prod = i2scan^.item.prod) and
                  (i1scan^.item.dp = i2scan^.item.dp);
         if match then 
	 begin 
	    new (last^.next);
	    last^.next^.next := nil; { JDA }
            last := last^.next;
            last^.item := i1scan^.item;
            last^.item.sym := i1scan^.item.sym + i2scan^.item.sym;
	 end; {if} 

         i2scan := i2scan^.next;
      end; {while} 

      i1scan := i1scan^.next;

   end; {while} 

   last^.next := nil;
   union := first^.next;   {skip dummy node}

end; {union}

{   copy (i)
given a list of items i, copy each node to a new list and return that.}

function copy (i : item1set) : item1set;
   var i1scan : item1set;
       first, last : item1set;
begin

   new (first);   {create leading dummy node}
   first^.next := nil; {JDA }
   last := first;
   i1scan := i;

   while i1scan <> nil do 
   begin 
      new (last^.next);
      last^.next^.next := nil; { JDA }
      last := last^.next;
      last^.item := i1scan^.item;

      i1scan := i1scan^.next;

   end; {while} 

   last^.next := nil;
   copy := first^.next;   {skip dummy node}
   
end; {copy}

{   insert (m)
given a new machine m, insert it in the new machine.}

function insert (m : lrmac) : lrmac;
   var scan : lrmac;
       match : boolean;
       templalr : lrmac;
begin
   scan := startlalr^.next;   {skip dummy}
   match := false;
   while (scan <> nil) and not match do 
   begin 
      if eqitemset (scan^.itemset, m^.itemset, true {treat like lr0}) then 
      begin 
	 match := true;
         scan^.itemset := union (m^.itemset, scan^.itemset);
         insert := scan;
      end; {if} 

      scan := scan^.next;

   end; {while} 

   if not match then 
   begin 
      new (templalr);
      templalr^.itemset := copy (m^.itemset); 
      templalr^.golist := copygo (m^.golist);
      templalr^.statenum := endlalr^.statenum + 1;
      templalr^.lalrset := nil; { JDA }
      templalr^.next := nil;
      endlalr^.next := templalr;
      endlalr := templalr;
      insert := endlalr;
   end; {if} 

end; {insert}

{   resolve
modify the goto pointers in the new machine so that they point to the new
rather than the old.}

function resolve : lrmac;
   var scan : lrmac;
      goscan : goptr;
begin
   scan := startlalr^.next;   {skip dummy}
   while scan <> nil do 
   begin 
      goscan := scan^.golist;
      while goscan <> nil do 
      begin 
	 goscan^.go := goscan^.go^.lalrset;
         goscan := goscan ^.next;
      end; {while} 
      
      scan := scan ^.next;
   end; {while} 

   resolve := startlalr^.next;

end; {resolve}

begin {lalrmac}
   scan := m;
   initinsert (startlalr, endlalr);
   
   while scan <> nil do 
   begin 
      scan^.lalrset := insert (scan);
      scan := scan^.next;
   end; {while} 

   lalrmac := resolve;

end; {lalrmac}

{----------------------------------------------------------------------
 ----------------------------------------------------------------------}

{   options
read and save options selected.}

procedure options ;
   var wt:wordtype;
begin
   shift (wt,lastword);
   if not strequal (wt, optnwd) then
   begin
      error (2 {'missing keyword <<options>>'})
   end
   else
   begin
      readword (lastword);
   end; {if}

   writeln (output, '     ');
   writeln (output, '     ');
   writeln (output, '   options in effect:');

   opts := [];

   shift (wt, lastword);
   while not strequal (wt, termwd) and 
         not strequal (lastword, eofwd) do
   begin
      shift (wt, lastword);
      if strequal (wt, fsmwd) then
      begin
	 opts := opts + [fsmopt];
      end
      else if strequal (wt, minwd) then
      begin
	 opts := opts + [minopt];
      end
      else if strequal (wt, slrwd) then
      begin
	 opts := opts + [slropt];
      end
      else if strequal (wt, lalrwd) then
      begin
	 opts := opts + [lalropt];
      end
      else if strequal (wt, lrwd) then
      begin
	 opts := opts + [lropt];
      end
      else if strequal (wt, fstfolwd) then
      begin
         opts := opts + [fstfolopt];
      end
      else if strequal (wt,tblwd) then
      begin
	 opts := opts + [tblopt];
      end
      else if strequal (wt, echowd) then
      begin
	 opts := opts + [echoopt];
      end
      else 
      begin
	 error (3 {'unrecognized option ', lastword});
         writeword (output, lastword);
         writeln (output);
      end; {if}

      writeln (output);
      write (output, '      ');
      shift (wt, lastword);
      writeword (output, wt);
      readword (lastword);
      shift (wt, lastword);

   end; {while}

   writeln (output);
   if opts = [] then
      error (70);

   if strequal (lastword, eofwd) then
   begin
      error (4 {'premature eof while reading options'});
   end; {if}
end; {options}

{   terminals
read the terminals.}

procedure terminals ;
   var
      count : integer;
      wt: wordtype;
begin
   shift (wt, lastword);
   if not strequal (wt, termwd) then
   begin
      error (5 {'missing keyword <<terminals>>'});
   end
   else
   begin
      readword (lastword);
   end; {if}

   count := 0;

   shift (wt, lastword);
   while not strequal (wt, nontermwd) and
         not strequal (lastword, eofwd) do
   begin
      count := count + 1;
      insertst ({word} lastword, {isterm} true, {pos} count);
      readword (lastword);
      shift (wt,lastword);
   end; {while}

   if strequal (lastword, eofwd) then
   begin
      error (6 {'premature eof while reading terminals'});
   end; {if}

   insertst (dolwd, true, count + 1);
   dolsym := count + 1;

   numt := count + 1;

end; {terminals}

{   nonterminals
read the nonterminals.}

procedure nonterminals ;
   var count : integer;
       wt : wordtype;
begin
   shift(wt,lastword);
   if not strequal (wt, nontermwd) then 
   begin 
      error (7 {'missing keyword <<nonterminals>>'});
   end 
   else 
   begin 
      readword (lastword);
   end; {if} 

   startlhssym := numt + 1;
   count := 0;

   shift (wt, lastword);
   while not strequal (wt, prodnwd) and 
         not strequal (lastword, eofwd) do
   begin
      count := count + 1;
      insertst ({word} lastword, {isterm} false, {pos} count + numt);
      readword (lastword);
      shift (wt, lastword);
   end; {while}

   if strequal (lastword, eofwd) then 
   begin 
      error (8 {'premature eof while reading nonterminals'});
   end; {if} 

   insertst (sprimewd, false, count + numt + 1);
   sprimesym := count + numt + 1;

   numnt := count + 1;

end; {nonterminals}

{   echo
print information obtained so far.}

procedure echo ;
   var i : integer;
       t : prodlist;
       wtsym:wordtype;



{   printtables: print production information}

procedure printtables;
   var i : integer;
       count : integer;
       rhs : prodlist;
begin
   writeln (output, '     ');
   writeln (output, '     ');

   writeln (output, '  prodlhstable : array [ 1 .. ', nump:3,
                                                          ' ] of integer;');
   writeln (output);
   for i := 1 to nump do
   begin
      writeln (output, 'prodlhstable [',i:3,'] := ',prodarray [i].lhs:3, ';');
   end;

   writeln (output);
   writeln (output, '     ');

   writeln (output, '  prodlentable : array [ 1 .. ', nump:3,
                                                           ' ] of integer;');
   writeln (output, '     ');
   for i := 1 to nump do
   begin
      count := 0;
      rhs := prodarray [i].rhlist;
      while rhs <> nil do
      begin
         count := count + 1;
         rhs := rhs^.next;
      end;

      writeln (output, 'prodlentable [',i:3,'] := ',count:3, ';');
   end;
   write (output, '     ');

end; {printtables}

begin  {echo}
   writeln (output, '     ');
   writeln (output, '     ');
   writeln (output, '-- terminals --');
   writeln (output);

   for i := 1 to numt do
   begin
      write (output, i:4, ' ');
      lookupn (wtsym, i);
      writeword (output, wtsym);
      writeln (output);
   end;


   writeln (output, '     ');
   writeln (output, '     ');
   writeln (output, '-- nonterminals --');
   writeln (output);

   for i := numt + 1 to numt + numnt do
   begin
      write (output, i:4, ' ');
      lookupn (wtsym, i);
      writeword (output, wtsym);
      writeln (output);
   end;


   writeln (output, '     ');
   writeln (output, '     ');
   writeln (output, '-- productions --');
   writeln (output);

   for i := 1 to nump do
   begin
      write (output, i:4, ' ');
      lookupn (wtsym,lhsn(i));
      writeword (output, wtsym);
      write (output, ' --> ');

      t := rhsn (i);
      while not donerhs (t) do
      begin
	 lookupn (wtsym, rhssym (t));
	 writeword (output, wtsym);
         write (output, ' ');
         t := nextrhs (t);
      end; {while}

      if (i = nump) then write (output, '$');

      writeln (output);
   end; {for}

      {first and follow sets}
   if fstfolopt in opts then
      printff;

      {production tables for real parser}
   printtables;
end; {echo}

{   gramcheck
verify usage of each symbol.}

procedure gramcheck;
   var i : symbol;
begin
      {check terminals:  never used on an lhs, used on some rhs.}   
   for i := 1 to numt - 1 do   {skip $}
   begin
      if symtab [i].usedlhs then
      begin
         error (50);   {term used on lhs}
	 intnotused := writesym (i);
         writeln (output);
      end;

      if not symtab [i].usedrhs then 
      begin 
	 error (51);   {term never used on rhs}
	 intnotused := writesym (i);
         writeln (output);
      end; {if} 

   end; {for}

      {check nonterminals:  used on some lhs, used on some rhs.
         start at (numt + 2), ie. skip the start symbol.}
   for i := numt + 2 to numt + numnt - 1 do 
   begin 
      if not symtab [i].usedlhs then 
      begin 
         error (52);   {nonterminal never used on lhs}
	 intnotused := writesym (i);
         writeln (output);
      end; {if} 

      if not symtab [i].usedrhs then 
      begin 
         error (53);   {nonterminal never used on rhs}
	 intnotused := writesym (i);
         writeln (output);
      end; {if} 

   end; {for} 
   
end; {gramcheck}

{--------------------------------------------------------------------}

{      productions routines:  productions, printprodhead, printprodact,
printprodtail, checkprod}


{   productions
read in the productions;  save semantics to the file parser.}

procedure productions;
   var pn : integer;
       prevlhs, cursym : symbol;
       wt:wordtype;


{   printprodhead
header information for the semantic routines.}

procedure printprodhead;
begin
   writeln (output, '     ');
   writeln (parser, '      procedure to handle actions associated with ',
         'the productions');
   writeln (parser);
   writeln (parser);

   writeln (parser, 'procedure perform (i : integer);');
   writeln (parser, '{   i is the # of the production reduced by.}');
   writeln (parser, 'begin');
   writeln (parser, '      case i of');
   writeln (parser);

end; {printprodhead}

{   printprodact
print the number pn as the index to a case statement for the production
semantics.}

procedure printprodact (pn : integer);
   var i : integer;
begin
   writeln (parser, pn:3, ' :   begin');   
   
   i := 0;
   readword (lastword);

   while not strequal (lastword, eopwd) and 
         not strequal (lastword, eofwd) do 
   begin 
      writeword (parser, lastword);
      write (parser, ' ');
      i := i + 1;
      if i mod 5 = 0 then 
         writeln (parser);
      readword (lastword);
   end; {while} 

   writeln (parser, '        end;');

end; {printprodact}

{   printprodtail
close off the semantic action procedure.}

procedure printprodtail;
begin
   writeln (parser);
   writeln (parser, '   end; {case}');
   writeln (parser, 'end;   {perform}');
   writeln (parser);
end; {printprodtail}

{   checkprod (lastlhs, cursym)
verify that each group of productions corresponding to some lhs
is read in all together.}

procedure checkprod (var lastlhs : symbol;  cursym : symbol);
begin
   if lastlhs <> cursym then 
   begin 
      if symtab [cursym].usedprod then 
      begin 
	 error (60);   {production out of place}
	 intnotused := writesym (cursym);
         writeln (output);
      end 
      else 
      begin 
	 symtab [cursym].usedprod := true;
      end; {if} 

   end; {if} 

   lastlhs := cursym;

end; {checkprod}

begin   {productions}
   shift (wt, lastword);
   if strequal (wt, prodnwd) then 
   begin 
      readword (lastword);
   end 
   else 
   begin 
      error (9 {'missing keyword <<productions>>'});
   end; {if} 

   (* printprodhead;  JDA *)
   initprod;
   pn := 0;
   prevlhs := emptysym;   {no symbols yet}
   
   while not strequal (lastword, eofwd) do
   begin
      nextprod;
      pn := pn + 1;

      cursym := lookupl (lastword, pn);
      checkprod (prevlhs, cursym);
      putlhs (cursym, pn);

      readword (lastword);  {arrow}
      readword (lastword);

      while not strequal (lastword, eopwd) and
            not strequal (lastword, eofwd) do
      begin
	 putrhs (lookupr (lastword, pn));
         readword (lastword);
      end; {while}

      (* printprodact (pn);  JDA *)

      lastrhs;
      readword (lastword);  {skip over eop}
   end; {while}

   (* printprodtail;   JDA *)

   nextprod;
   pn := pn + 1;
   sprimepn := pn;
   
   putlhs (sprimesym, pn);
   putrhs (startlhssym);
   lastrhs;

   nump := pn;

end; {productions}

{---------------------------------------------------------------
 ---------------------------------------------------------------}

{      slr, lalr, lr, notlr      functions to check for each.}
function slr : boolean;
   var m : lrmac;
begin

   m := lr0items;

   if fsmopt in opts then
      wrmac (lr0items, true);

   slr := checklr (lr0items, slrtype);
end; {slr}

function lalr (m : lrmac) : boolean;
   var m2 : lrmac;
begin

   m2 := lalrmac (m);

   if fsmopt in opts then
      wrmac (m2, false {not slr});

   lalr := checklr (m2, lalrtype);

end; {lalr}

function lr (m : lrmac) : boolean;

begin
   
   if fsmopt in opts then
      wrmac (m, false {not slr});

   lr := checklr (m, lrtype);

end; {lr}

procedure notlr;

begin

   writeln (output, '     ');
   writeln (output);
   writeln (output, '   the given grammar is neither slr(1), lalr(1), ',
      'nor lr(1)');
   writeln (output);

end; {procedure}

{   driver
main procedure for the whole program.}

procedure driver;
   var 
      isslr, islalr, islr : boolean;
      m : lrmac;
begin
   errorfree := true;
   initio;
   readword (lastword);
   (* rewrite (parser); JDA *)
   initst;

   options;

   terminals;  nonterminals;  productions;

   createfirst;    
   createfollow;

   if echoopt in opts then 
      echo;

   gramcheck;

   if not errorfree then
   begin
      writeln (output);
      writeln (output, 'errors were found -- no more processing will be done');
      writeln (output);
   end
   else
   begin
      isslr := false;
      islalr := false;
      islr := false;
      
      if (slropt in opts) or (minopt in opts) then
	 isslr := slr;

      if ([lalropt, lropt] * opts <> []) or 
         (minopt in opts) and not isslr then
           m := lr1items;

      if (lalropt in opts) or (minopt in opts) and not isslr then
	 islalr := lalr (m);

      if (lropt in opts) or (minopt in opts) and not islalr and not isslr then
	 islr := lr (m);

      if (minopt in opts) and not isslr and not islalr and not islr then
	 notlr;
   end;
end;  {driver}

begin  {main}

   driver;  

end.
