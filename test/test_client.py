import time
import socket

INPUTFILE = "./inputFile.csv"
OUTPUTFILE = "./outputFile.csv"
SERVER_OUTPUT = "./testOutput.csv" # this file is continuously updated by the running server

def parse_file(filename):
    scenarios = {}
    current_scenario = None
    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if line.startswith('#scenario'):
                current_scenario = line[1:].strip()  # Remove '#' and any leading/trailing whitespace
                scenarios[current_scenario] = []
            elif line and not line.startswith('#') and current_scenario:
                scenarios[current_scenario].append(line)
    return scenarios

def run_scenario(client_socket, addr, scenario_lines):
    for line in scenario_lines:
        message = line.encode('utf8')
        print(f"Sending: {message}")
        client_socket.sendto(message, addr)
        time.sleep(0.1)  # Small delay to allow processing

def read_server_output(start_line):
    with open(SERVER_OUTPUT, 'r') as f:
        lines = f.readlines()[start_line:]
    return [line.strip() for line in lines if line.strip()]

def compare_scenario_results(scenario, expected, actual):
    print(f"\nResults for {scenario}:")
    if not actual:
        print("FAIL: No output received for this scenario")
        return

    all_match = True
    max_lines = max(len(expected), len(actual))

    for i in range(max_lines):
        if i < len(expected) and i < len(actual):
            exp = expected[i]
            act = actual[i]
            if exp != act:
                print(f"FAIL on line {i+1}: Expected '{exp}', got '{act}'")
                all_match = False
            else:
                print(f"PASS on line {i+1}: '{exp}'")
        elif i < len(expected):
            print(f"FAIL on line {i+1}: Expected '{expected[i]}', got no output")
            all_match = False
        else:
            print(f"FAIL on line {i+1}: Expected no output, got '{actual[i]}'")
            all_match = False

    if all_match:
        print("SUCCESS: All lines match expected output")
    else:
        print(f"FAIL: Mismatch in output. Expected {len(expected)} lines, got {len(actual)} lines")

def main():
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    addr = ("127.0.0.1", 12345)

    input_scenarios = parse_file(INPUTFILE)
    output_scenarios = parse_file(OUTPUTFILE)

    start_line = 0
    for scenario, lines in input_scenarios.items():
        print(f"\nRunning {scenario}")
        run_scenario(client_socket, addr, lines)

        # Give the server some time to process and write output
        time.sleep(1)

        # Read new output since last scenario
        new_output = read_server_output(start_line)
        start_line += len(new_output)

        # Compare results for this scenario
        if scenario in output_scenarios:
            compare_scenario_results(scenario, output_scenarios[scenario], new_output)
        else:
            print(f"FAIL: {scenario} not found in expected output")

if __name__ == "__main__":
    start = time.time()
    main()
    end = time.time()
    print(f"\nElapsed {(end-start)*10**3:.2f} millisec.")