procedure LEGAL is     -- this is a comment
    i : integer;       -- declarations go here
    ZERO : constant integer := 0;
    begin
        i := 3;
        while not (i = 0) loop -- while <exp> loop <body> end loop;
            declare pos : boolean; -- more decls in blocks
                      j : integer;
            begin
                put("Enter an integer");
                get(i);
                pos := i > 0;
                if pos then put("entered positive"); end if;
                -- if <exp> then <statements> end if;
                if i < 0 then put("entered negative"); end if;
            end;
        end loop;
    end LEGAL;
