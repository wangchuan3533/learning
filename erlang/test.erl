-module(test).
-compile(export_all).

fac(0) -> 1;
fac(N) -> N * fac(N - 1).

bump([]) -> [];
bump([H|T]) -> [H+1|bump(T)].

average(L) -> sum(L)/len(L).
len([]) -> 0;
len([_|T]) -> 1+len(T).

even([]) -> [];
even([H|T]) when H rem 2 == 0 -> [H|even(T)];
even([_|T]) -> even(T).

member(_, []) -> false;
member(H, [H|_]) -> true;
member(H, [_|T]) -> member(H, T).

sum(L) -> acc_sum(L, 0).
acc_sum([], S) -> S;
acc_sum([H|T], S) -> acc_sum(T, S+H).
