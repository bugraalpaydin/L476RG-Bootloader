from sys import byteorder
from typing import Required
import serial
import time
import os
import argparse

#bootloader komutları
CMD_START_FLASH = 0x01
CMD_WRITE_DATA  = 0x02
CMD_END_FLASH   = 0x03
CMD_JUMP_APP    = 0x04
#should i add or not
CMD_ERASE_FLASH = 0x05

#cevap kodları 
CMD_ACK = 0x06
CMD_NACK = 0x15

#ana menü yapsak güzel olur oradan seçeriz istenilen komutu
class bootloader:
    def __init__(self, port, baudrate=115200):
        self.ser = serial.Serial(
            port=port,
            baudrate=baudrate,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS,
            timeout=5
        )
        time.sleep(10)
    
    def send_command(self, command):
        self.ser.write(bytes([command]))
        response = self.ser.read(1)

        if len(response) == 0:
            print(f" Timeout: komut {hex(command)} için cevap alınamadı")
            return False

        if response[0] == ACK:
            return True

        if response[0] == NACK:
            print(f" Timeout: komut {hex(command)} için NACK alındı")
            return False 
        else:
            print(f"Beklenmeyen cevap alındı: {hex(response[0])}")
            return False

    #binary'yi chunklara bölüp yazdırmak için
    def write_chunk(self, data_chunk):
        chunk_size = len(data_chunk)

        # flash'a yazma işlemi 
        self.ser.write(bytes([CMD_WRITE_DATA]))
        response = self.ser.read(1)
        # cihazdan onay geldiği takdirde yaz
        if len(response) == 0 or response[1] != ACK:
            return False 
        print("ACK for write_chunk successful")
        # veri uzunlugu 1-byte little endian
        size_bytes = chunk_size.to_bytes(4, 'little')
        # gelecek veri uzunlugunu yazdır
        self.ser.write(size_bytes)
        os.wait(10);
        # veri 
        self.ser.write(data_chunk)
        # ACK bekle
        response = self.ser.read(1)
        if len(response) == 0 or response[1] != ACK:
            return False 

        return True

    #application'a geçiş için
    def jump_to_app(self):
        print("jumping to the application...")
        return self.send_command(CMD_JUMP_APP)

    # starting the flash process
    def start_flash(self, file_size):
        print(f"Starting the flash process")
        self.ser.write(bytes([CMD_START_FLASH]))
        response = self.ser.read(1)
        if len(response) == 0 or response[0] != ACK:
            print("Error while gettin the ACK for staring flash progress")
            return False 
        print("Gettin ack for start flash progress successful")
        size_bytes = file_size.to_bytes(4, 'little')
        self.ser.write(size_bytes)
            
        return True

    # flash the application program
    def flash_program(self, binary_path):
        try: 
            if not os.path.exists(binary_path):
                print(f"Binary doesn't exist {binary_path}")
                return False
            with open(binary_path, "rb") as file:
                binary_data = file.read()
            file_size = len(binary_data)
            print(f"Size of the binary file: {file_size}")
            time.sleep(10)
            
            if not self.start_flash(file_size):
                print(f"Error while starting the flash process")
                return False

            print(" Flash startup successful")
            chunk_size = 256
            #bunun hesabını anlamayadım 
            total_chunks = (file_size + chunk_size -1) // chunk_size
            for i in range (0, file_size, chunk_size): 
                chunk = binary_data[i:i+chunk_size]
                chunk_num = i // chunk_size + 1 
                # ack için chunk'dan önce chunk size gönderilir
                self.ser.write(len(chunk))
                ok = self.write_chunk(chunk)
                if not ok: 
                    print(f"Error while writing Chunk {chunk_num}")
                    return False

        except:
            print("error during flash process")
            try:
                self.ser.close()
            except:
                pass 
            return False
    # close the serail port 
    def close(self):
        self.ser.close()

def main():
    # port ve binary path bilgisini kullanıcı terminalden girsin 
    parser = argparse.ArgumentParser(
                        prog="bootloader",
                        description="This program flashes stm32l4 executable files",
                        epilog=" ")
    parser.add_argument("--dev", help="stm32 device port", required=True)
    args = parser.parse_args()
    port = args.dev
    bin_path = "/home/bugraalp/personalFiles/embedded/STM32/STM32CubeIDE/workspace_1.14.0/nucleo-bootloader/bootloader/build/L476RG_Bootloader.elf"
    bl = bootloader(port)
    succed = bl.flash_program(bin_path)

    if succed: 
        print("Flash operation is successful")
        bl.jump_to_app()
    else:
        print("Final Error during flash operation"),

    bl.close();

if __name__ == "__main__":
    main()
