import sys
import re

insts = []

#命令に関する処理を行う
def init():
    with open('insts','r') as f:
        for line in f.read().splitlines():
            #コメントは読み飛ばす
            if(len(line) == 0 or line[0] == '*'):
                continue

            token = re.split(' ',line)
            inst = token[0]
            opcode = token[1]
            args_types = token[2:]
            
            insts.append([inst,opcode,args_types])

#命令の名前からinstsの中身を検索する
def searchinst(instname):
    for i in range(len(insts)):
        if(instname == insts[i][0]):
            return i

    print("命令がありませんでした")
    return -1

def makebinary(inst,args):
    index = searchinst(inst)
    opcode = insts[index][1]
    args_types = insts[index][2]

    args_fixed = []

    #最終的に得られるバイナリ
    code = int(opcode,2)
    #(ポインタは5bit分)
    pointer = 5

    for i in range(len(args_types)):
        #int型の場合
        if(args_types[i] == 'I'):
            #argsの最初はiでないといけない
            #assert 条件がTrueのとき,条件がFalseのときに出力されるメッセージ
            assert args[i][0] == 'i','I型の最初がiではありませんでした'
            assert int(args[i][1:]) < 2**16,'overflow:16bitの範囲を数が超えました。'

            args_fixed.append(args[i][1:])
            
            code += int(args[i][1:]) << pointer
            pointer += 16

        #registerの場合
        elif(args_types[i] == 'R'):
            #argsの最初はxでないといけない
            assert args[i][0] == 'r','R型の最初がrではありませんでした'
            assert int(args[i][1:]) < 2**5,'overflow:5bitの範囲を数が超えました。'
            args_fixed.append(args[i][1:])
            code += int(args[i][1:]) << pointer
            pointer += 5

        #メモリの位置の場合
        elif(args_types[i] == 'M'):
            #argsの最初はmでないといけない
            assert args[i][0] == 'm','M型の最初がmではありませんでした'
            assert int(args[i][1:]) < 2**16,'overflow:16bitの範囲を数が超えました。'

            args_fixed.append(args[i][1:])

            code += int(args[i][1:]) << pointer
            pointer += 16
        #shamtの場合
        elif(args_types[i] == 'S'):
            #argsの最初はsでないといけない
            assert args[i][0] == 's','S(shamt)型の最初がsではありませんでした'
            assert int(args[i][1:]) < 2**5,'overflow:16bitの範囲を数が超えました。'

            args_fixed.append(args[i][1:])

            code += int(args[i][1:]) << pointer
            pointer += 5

        #型が存在しない場合
        else:
            print("insts:型の定義が存在しません。")

    print(f'{code:032b}')
    return code

#ソースコードすべてのラベルを取得する。
def get_all_labels(path):
    labels = []
    pc = 0
    with open(path,'r') as f:
        for line in f.read().splitlines():
            #コメントアウト、空の行の場合読み飛ばす
            if(len(line) == 0 or line[0:2] == '//' or line[0] == '#'):
                continue

            #ラベルを取得
            if(line[-1] == ':'):
                #ラベルのリストにラベル名と、プログラムカウンタ
                labels.append((line[:-1],str(pc)))
                continue

            pc = pc + 4

    return labels

def get_src(path=sys.argv[1]):
    bin = []

    labels = get_all_labels(path)

    with open(path,'r') as f:
        for line in f.read().splitlines():
            #コメントアウト、空の行、ラベルの場合読み飛ばす
            if(len(line) == 0 or line[0:2] == '//' or line[-1] == ':' or line[0] == '#'):
                continue

            token = re.split('[, :]',line)

            inst = token[0]
            args = token[1:]

            print(inst,args)

            #ラベルの部分を数字に置き換える
            for i in range(len(args)):
                #もし引数がラベルの名前リストの中に入っていたら
                if(args[i] in [j[0] for j in labels]):
                    #argsを置き換える
                    args[i] = 'm'+labels[[j[0] for j in labels].index(args[i])][1]
                

            bin.append(makebinary(inst,args))

    with open('untitled.bin','wb') as f:
        for i in bin:
            f.write(i.to_bytes(4,'little'))


if __name__ == '__main__':
    init()
    get_src()