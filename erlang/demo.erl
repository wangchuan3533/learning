-module(demo).
-export([double/1]).
% This is a demo

double(X) ->
	times(X, 2).
times(X, Y) ->
	X * Y.

