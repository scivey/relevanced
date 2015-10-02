from func_test_lib.newsgroups import insert_data, test
import sys

def main():
    if len(sys.argv) >= 2:
        arg = sys.argv[1]
        if arg == 'insert':
            insert_data()
            return
        elif arg == 'test':
            test()
            return
    raise Exception('specify insert|test')

if __name__ == '__main__':
    main()
