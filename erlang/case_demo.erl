% case
-module(case_demo).
-export([convert/1]).
convert(Day) ->
case Day of
monday -> 1;
tuesday -> 2;
wednesday -> 3;
thursday -> 4;
friday -> 5;
sataday -> 6;
sunday -> 7;
Other -> {error, unknown_day}
end.
