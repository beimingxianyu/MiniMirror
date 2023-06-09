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
    file_data = file_data.replace("@hardware_info_mac_address@", get_mac_address())
    file_data = file_data.replace("@hardware_info_mac_address_16@", get_mac_address_16())
    return file_data


def cmake_config_system_info(cmake_config_system_info_file_in: str, cmake_config_system_info_file_out: str):
    file = open(cmake_config_system_info_file_in, "r", encoding="utf8")
    if file.readable():
        file_data = file.read()
        file_data = config_mac_address(file_data)
    else:
        raise FileNotFoundError("\"cmake_config_system_info_file_in\" can't read.")
    file.close()
    file = open(cmake_config_system_info_file_out, "w", encoding="utf8")

    if file.writable():
        file.write(file_data)
    else:
        raise FileExistsError("\"cmake_config_system_info_file_out\" can't write.")
    file.close()
    return


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='manual to this script')
    parser.add_argument("--function", type=str, default="")
    parser.add_argument("--cmake_config_system_info_file_in", type=str, default="")
    parser.add_argument("--cmake_config_system_info_file_out", type=str, default="")
    args = parser.parse_args()
    if args.function != "":
        eval(args.function)
    if args.cmake_config_system_info_file_in != "" and args.cmake_config_system_info_file_out != "":
        cmake_config_system_info(args.cmake_config_system_info_file_in, args.cmake_config_system_info_file_out)
