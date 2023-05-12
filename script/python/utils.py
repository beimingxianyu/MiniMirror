import uuid
import argparse


def get_mac_address_16():
    mac = uuid.UUID(int=uuid.getnode()).hex[-12:]
    max_16 = int(mac, 16)
    return hex(max_16)


def get_mac_address():
    mac = uuid.UUID(int=uuid.getnode()).hex[-12:]
    return ":".join([mac[e:e + 2] for e in range(0, 11, 2)])


def get_mac_address_console():
    print(get_mac_address())


def config_mac_address(file_data: str):
    start1 = file_data.find("@hardware_info_mac@")
    start2 = file_data.find("@hardware_info_mac_16")
    file_data[start1, start1 + 19] = get_mac_address()
    file_data[start2, start2 + 19] = get_mac_address_16()


def cmake_config_system_info(cmake_config_system_info_file: str):
    file = open(cmake_config_system_info_file, "w+")
    if file.writable():
        config_mac_address()
    else:
        raise FileNotFoundError("\"cmake_config_system_info_file\" not find")
    return


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='manual to this script')
    parser.add_argument("--function", type=str, default="")
    parser.add_argument("--cmake_config_system_info_file", type=str, default="")
    args = parser.parse_args()
    eval(args.function)
    if args.cmake_config_system_info_file != "":
        cmake_config_system_info(args.cmake_config_system_info_file)
