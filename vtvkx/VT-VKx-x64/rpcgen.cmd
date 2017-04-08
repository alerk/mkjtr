cd ..\mingw64
del CControl.h CControl_xdr.c CControl_clnt.c CControl_svc.c
%MSYS2_HOME%\usr\local\bin\rpcgen -C -N -M -h -o CControl.h CControl.x
%MSYS2_HOME%\usr\local\bin\rpcgen -C -N -M -c -o CControl_xdr.c CControl.x
%MSYS2_HOME%\usr\local\bin\rpcgen -C -N -M -l -o CControl_clnt.c CControl.x
%MSYS2_HOME%\usr\local\bin\rpcgen -C -N -M -m -o CControl_svc.c CControl.x