from collections import deque, namedtuple
from typing import Dict, List, Tuple
from uuid import UUID, uuid4
from operator import gt, lt, eq
from math import prod

Packet = namedtuple("Packet", "version type_id parent payload value depth")

class Decoder():

    def __init__(self, data:str) -> None:
        
        # Convert the hexadecimal text to bytes
        raw_bytes = bytes.fromhex(data)

        # Pack into a queue all bits from those bytes
        self.bits = deque()
        for value in raw_bytes:
            self.bits.extend(
                (value & (1 << n)) >> n
                for n in range(7, -1, -1)
            )
        
        # Store the packets and associate an unique ID to each
        self.packets:Dict[UUID,Packet] = {}
        
        # Checksum of the packets versions
        self.version_sum = 0

        # Operators for each packet type:
        # True means that it takes an iterable
        # False means that it takes individual parameters
        Oper = namedtuple("Operator", "function takes_iter")
        self.operators = {
            0: Oper(sum, True),     # Sum of all subpackets
            1: Oper(prod, True),    # Product of all subpackets
            2: Oper(min, True),     # Minimum of all subpackets
            3: Oper(max, True),     # Maximum of all subpackets
            5: Oper(gt, False),     # subpacked_a > subpacket_b
            6: Oper(lt, False),     # subpacked_a < subpacket_b
            7: Oper(eq, False)      # subpacked_a == subpacket_b
        }
    
    def next_bits(self, amount:int) -> int:
        """Unpack an amount of bits from the queue."""
        output = 0
        for bit in range(amount):
            try:
                output <<= 1
                output |= self.bits.popleft()
            except IndexError:
                raise StopDecoding("Reached the end of the bit stream.")
        
        return output
    
    def parse_packet(self, parent:Packet=None, depth:int=0) -> Tuple[Packet, int]:
        """Get all the data from the packet and calculate its value."""
        
        uuid = uuid4()  # Unique ID for the packet
        bit_count = 0   # Length in bits of the packet

        # Parse the header
        version = self.next_bits(3)
        type_id = self.next_bits(3)
        bit_count += 6
        self.version_sum += version

        # If the packet contains a literal value
        if type_id == 4:
            
            # Parse the value until the leading bit of the fragment is 0
            payload = 0
            separator = 1
            while separator == 1:
                separator = self.next_bits(1)
                payload <<= 4
                payload |= self.next_bits(4)
                bit_count += 5
            value = payload
        
        # If the packet is an operator
        else:
            
            # Get the logic of the length header
            length_type = self.next_bits(1)
            bit_count += 1
            payload:List[Packet] = []
            
            # When the length is the bit length of all subpackets
            if length_type == 0:
                max_bits = self.next_bits(15)
                bit_count += 15
                decoded_size = 0
                
                # Parse subpackets until the length is reached
                while decoded_size < max_bits:
                    subpacket, packet_size = self.parse_packet(parent=uuid, depth=depth+1)
                    payload += [subpacket]
                    decoded_size += packet_size
                    bit_count += packet_size
                assert decoded_size == max_bits
            
            # When the length is the amount of subpackets
            elif length_type == 1:
                max_packets = self.next_bits(11)
                bit_count += 11

                # Parse subpackets until the length is reached
                for _ in range(max_packets):
                    subpacket, packet_size = self.parse_packet(parent=uuid, depth=depth+1)
                    payload += [subpacket]
                    bit_count += packet_size
            
            # Determine the operation of the packet and its operands
            my_oper = self.operators[type_id]
            sub_values = (packet.value for packet in payload)
            
            # Perform the operation on the operands
            if my_oper.takes_iter:
                value = my_oper.function(sub_values)
            else:
                value = my_oper.function(*sub_values)
        
        # Store the packet data in a Packet object
        packet = Packet(version, type_id, parent, payload, value, depth)
        assert uuid not in self.packets  # Yes, I do know that I am being overzealous here :-)
        self.packets[uuid] = packet

        # Return the Packet object and its bit length
        return (packet, bit_count)
    
    def decode(self) -> None:
        """Decode the values of all packets in the data stream."""
        
        while len(self.bits) > 10:  # A packet contain a minimum of 11 bits (6 for the header, 5 for the payload of a literal)
            try:
                self.parse_packet()
            except StopDecoding:
                break


class StopDecoding(Exception):
    """Signals that the decoding ended."""


if __name__ == "__main__":

    with open("input.txt", "rt") as file:
        raw_text = file.read().strip()
    
    transmission = Decoder(raw_text)
    transmission.decode()
    
    print(f"Part 1: {transmission.version_sum}")

    outer_value, = (packet.value for packet in transmission.packets.values() if packet.depth == 0)
    print(f"Part 2: {outer_value}")