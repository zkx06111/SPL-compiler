#include <iostream>

#include "TypeUtil.h"

/*
program ttt(input, output);
const
    con = 123;
type
    Enum1 = (EA, EB, EC);
    Rec = record
        RecA: integer;
        RecB: real;
        RecC: EA .. EB;
    end;
var
    rrr: Rec;
    arr: array[0 .. 9] of boolean;

function func(n: integer): integer;
begin
    func := n * n;
end;

procedure proc(n: integer);
begin
    n := con;
end

begin
    arr[0] := false;
    rrr.RecA := func(2);
end.
*/

int main() {
    sem::sym_t.NewConst("con", 123);

    auto enm = sem::sym_t.NewEnum({ "EA", "EB", "EC"}).first;
    sem::sym_t.NameType("Enum1", enm);
    auto rec = sem::sym_t.NewRecord({
        {"RecA", sem::Type::Int()},
        {"RecB", sem::Type::Real()},
        {"RecC", enm}
    }).first;
    sem::sym_t.NameType("Rec", rec);

    sem::sym_t.NewVariable("rrr", rec);
    auto arr = sem::sym_t.NewArray(sem::sym_t.NewIntSubrange(0, 9).first,
        sem::Type::Bool()).first;
    sem::sym_t.NewVariable("arr", arr);

    sem::sym_t.NewFunc("func", sem::Type::Int(), {{"n", sem::Type::Int()}});

    sem::sym_t.NewProc("proc", {{"n", sem::Type::Int()}});

    std::cout << "end" << std::endl;

    return 0;
}