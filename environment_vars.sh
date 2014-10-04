#!/bin/bash

# source this file to setup export vars for building, running, and testing

export INSTRUMENTOR_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
#TODO: rename INSTRUMENTOR_LIB_PATH to be more meaningful
export INSTRUMENTOR_LIB_PATH="$INSTRUMENTOR_PATH""/src"
export BLAMEANALYSIS_LIB_PATH="$INSTRUMENTOR_PATH""/Release+Asserts/lib"
export MONITOR_LIB_PATH="$INSTRUMENTOR_PATH""/MonitorPass"

if [ "$(readlink "$INSTRUMENTOR_PATH""/.git/hooks")" == "" ]
then
	echo "WARNING: Your git hooks are not connected - please do not commit"
	echo "Run:"
	# makes symlink for git hooks path
	echo "rm -R "$INSTRUMENTOR_PATH"/".git/hooks""
	echo "ln -s "$INSTRUMENTOR_PATH"/".git_hooks" "$INSTRUMENTOR_PATH"/".git/hooks""
fi

shared_lib="so"
if [ "$platform" == "Darwin" ]
then
	shared_lib="dylib"
fi

export SHARED_LIB_EXTENSION=$shared_lib

function get_pkg_var () { # (pkg-config name, overwriting variable, overwriting value, pkg-config var to get)
	if [ -n "$2" ]
	then
		echo "$3"
		return
	fi
	pkg-config "$1" --variable="$4"
}

function get_lib_var () { # (pkg-config name, overwriting variable, overwriting value)
	get_pkg_var "$1" "$2" "$3" "libdir"
}

function get_include_var () { # (pkg-config name, overwriting variable, overwriting value)
	get_pkg_var "$1" "$2" "$3" "includedir"
}

export GLOG_LIB_PATH="$(get_lib_var 'libglog' "$GLOG_PATH" "$GLOG_PATH/lib")"
export GLOG_INCLUDE_PATH="$(get_include_var 'libglog' "$GLOG_PATH" "$GLOG_PATH/include")"

# this var is probably not necessary, but retaining for compatibility
export UNWIND_LIB_PATH="$(get_lib_var 'libunwind' "$LIBUNWIND_PATH" "$LIBUNWIND_PATH/lib")"
export UNWIND_INCLUDE_PATH="$(get_include_var 'libunwind' "$LIBUNWIND_PATH" "$LIBUNWIND_PATH/lib")"

# LLVM_LIB_PATH
# must contain gold plugin
# (which must be in the same path as other llvm libs per clang reqs)
if [ -z "$GOLD_LLVM_BIN" ]
then
	export LLVM_LIB_PATH="$(llvm-config --libdir)"
	export LLVM_BIN_PATH="$(dirname `which clang++`)" # a decent heuristic
else
	export LLVM_LIB_PATH=$GOLD_LLVM_BIN
	export LLVM_BIN_PATH=$GOLD_LLVM_BIN
fi


export GPERFTOOLS_LIB_PATH="$(get_lib_var "libprofiler" "$PROFILER_PATH" "$PROFILER_PATH/lib")"

export GLOG_log_dir="${GLOG_log_dir:-"/tmp/$(basename $INSTRUMENTOR_PATH)/logs"}"
if [ ! -d "$GLOG_log_dir" ]
then
	echo "Making log dir..."
	mkdir -p "$GLOG_log_dir"

fi
export CPUPROFILE="${CPUPROFILE:-""}"

# all libs are in the ld path now
export LD_LIBRARY_PATH="$INSTRUMENTOR_LIB_PATH/:$GPERFTOOLS_LIB_PATH/:$GLOG_LIB_PATH/:$UNWIND_LIB_PATH/:$MONITOR_LIB_PATH/:$BLAMEANALYSIS_LIB_PATH/:$LD_LIBRARY_PATH"
