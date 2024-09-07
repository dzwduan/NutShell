import re
from itanium_demangler import parse as demangle

def process_log_file(input_file_path, output_file_path):
    symbol_dict = {}
    pattern = re.compile(r'(\S+)\s+[BTU]\s+(\S+)')

    with open(input_file_path, 'r') as file:
        for line in file:
            match = pattern.search(line)
            if match:
                address, symbol = match.groups()
                try:
                    ast = demangle(symbol)
                    demangled_symbol = str(ast) if ast else symbol
                except Exception as e:
                    demangled_symbol = symbol
                symbol_dict[symbol] = demangled_symbol

    with open(output_file_path, 'w') as file:
        for key, value in symbol_dict.items():
            file.write(f"{key}: {value}\n")

# 示例使用
input_log_file_path = 'log'
output_log_file_path = 'processed_log'
process_log_file(input_log_file_path, output_log_file_path)