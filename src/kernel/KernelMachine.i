 %module(directors="1") CKernelMachine
%{
 #include "kernel/KernelMachine.h"
%}

%include "lib/common.i"

%feature("director");
/* %feature("director") CKernelMachine; */

%include "classifier/Classifier.i"
%include "kernel/KernelMachine.h"
