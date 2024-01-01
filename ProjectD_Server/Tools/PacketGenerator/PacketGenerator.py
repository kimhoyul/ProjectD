import argparse
import jinja2
import ProtoParser

def main():
    
    arg_parser = argparse.ArgumentParser(description = 'PacketGenerator')
    arg_parser.add_argument('--path', type=str, default='D:/Unreal/Project/ProjectD/ProjectD_Server/Common/Protobuf/bin/Protocol.proto', help='proto path')
    arg_parser.add_argument('--output', type=str, default='TestPacketHandler', help='output file')
    arg_parser.add_argument('--recv', type=str, default='C_', help='recv convention')
    arg_parser.add_argument('--send', type=str, default='S_', help='send convention')
    args = arg_parser.parse_args()
    
    parser = ProtoParser.ProtoParser(1000, args.recv, args.send)
    parser.parse_proto(args.path)
    
    # load in the template file
    file_loader =jinja2.FileSystemLoader('Templates')
    # save the template environment
    env = jinja2.Environment(loader=file_loader)

    # load the PacketHandler.h
    template = env.get_template('PacketHandler.h')
    # variables to be used in the template, and render the template
    output = template.render(parser = parser, output=args.output)

    f = open(args.output+'.h', 'w+')
    f.write(output)
    f.close()
    
    print(output)
    return

if __name__ == "__main__":
    main()
    

