pushd %~dp0
protoc.exe -I=./ --cpp_out=./ ./Enum.proto
protoc.exe -I=./ --cpp_out=./ ./Struct.proto
protoc.exe -I=./ --cpp_out=./ ./Protocol.proto
IF ERRORLEVEL 1 PAUSE

XCOPY /Y Enum.pb.h "..\..\..\ProjectD_Server"
XCOPY /Y Enum.pb.cc "..\..\..\ProjectD_Server"
XCOPY /Y Struct.pb.h "..\..\..\ProjectD_Server"
XCOPY /Y Struct.pb.cc "..\..\..\ProjectD_Server"
XCOPY /Y Protocol.pb.h "..\..\..\ProjectD_Server"
XCOPY /Y Protocol.pb.cc "..\..\..\ProjectD_Server"

XCOPY /Y Enum.pb.h "..\..\..\DummyClient"
XCOPY /Y Enum.pb.cc "..\..\..\DummyClient"
XCOPY /Y Struct.pb.h "..\..\..\DummyClient"
XCOPY /Y Struct.pb.cc "..\..\..\DummyClient"
XCOPY /Y Protocol.pb.h "..\..\..\DummyClient"
XCOPY /Y Protocol.pb.cc "..\..\..\DummyClient"