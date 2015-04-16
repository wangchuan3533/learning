# .bashrc

# User specific aliases and functions

# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi
export LANG="en_US.UTF-8"
alias xgrep="find . -name '*.cpp' -o -name '*.cc' -o -name '*.c' -o -name '*.php' -o -name '*.conf' -o -name '*.h' -o -name '*.lua' | xargs grep -n --color"

