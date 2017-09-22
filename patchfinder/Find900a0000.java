import java.io.*;
import java.util.*;

public class Find900a0000 {
	private static final long ldr_Rx_pc_Y_mask = 0xE59F0000;
	private static long mov_Rx_0x04000001_mask = 0xE3A00341;
	public static void main(String[] args) throws Exception { // yolo
		String replace900a0028 = "0x00010105";
		boolean noncas = args[1].substring(0,1).equalsIgnoreCase("n");
		if(noncas) {
			replace900a0028 = "0x00000000";
			mov_Rx_0x04000001_mask = 0xE3A00000; // now it is mov Rx, 0
		}
		Scanner sc = new Scanner(new File(args[0]));
		String line = "";
		ArrayList<String> addrs = new ArrayList<String>();
		ArrayList<String> addrs2c = new ArrayList<String>();
		while(sc.hasNextLine()) {
			line = sc.nextLine();
			if(line.contains(":\t900a0000 \t")) {
				addrs.add("; 0x"+line.split(":")[0]);
			} else if(line.contains(":\t900a002c \t")) {
				addrs2c.add("; 0x"+line.split(":")[0]);
			}
		}
		sc.close();
		
		sc = new Scanner(new File(args[0]));
		line = "";
		while(sc.hasNextLine()) {
			line = sc.nextLine();
			for(String s : addrs2c) {
				if(line.endsWith(s) && line.contains("\tldr\tr")) {
				}
				// fixme
				/*
				 1095cdec: e59f3bbc	ldr	r3,[1095d9b0] = 900a002c
				 1095cdf0: e5933000	ldr	r3,[r3]
				 */
				// honestly it might be a better idea to detect 900A0008 (W): Write a 2 to cause a hardware reset

			}
			for(String s : addrs) { // see if it is reading one of the addresses holding 900a0000 we found earlier
				// keep looking until the next time that register is written, bc it would no longer hold 900a0000
				if(line.endsWith(s) && line.contains("\tldr\tr")) {
					// find the place where it loads 900a0000 into a register, and what register
					// so we can replace the register it loads to with the one used to load 900a0028
					String registerLoad900a0000 = line.split("\tldr\t")[1].split(",")[0];
					int pcOffset900a0000 = Integer.parseInt(line.split("\\[")[1].split("\\]")[0].split("#")[1]); // get number from [pc, #whatever]
					String patchTargetLoad900a0000 = line.split(":")[0]; // i.e. the place where it loads 900a0000 into a register
					String addrLoad900a0000 = s.split("; 0x")[1]; // i.e. the place it GETS the value "900a0000" from
					String ldrit = "\tldr\t"+registerLoad900a0000+", ";
					String movit = "\tmov\t"+registerLoad900a0000+", ";
					boolean found28 = false;
					boolean found2c = false;
					do {
						line = sc.nextLine();
						// now, check for relative access to 0x28 & 0x2c bytes away from 900a0000 register
						String str28 = ", ["+registerLoad900a0000+", #40]\t; 0x28";
						String str2c = ", ["+registerLoad900a0000+", #44]\t; 0x2c";
						if(line.contains("\tldr\tr")) {
							// now, save the register and address where it references THAT register + 0x28 or 2c
							if(line.contains(str28)) {
								found28 = true;
								/* there are different patches for 900a0028 because 0x00010105 does not fit in arm mov
								 * basically
								 * change the code that loads 900a0000 value to load the value to the same register that LATER would have loaded 900a0028
								 * change the value at the address it originally loaded 900a0000 from, to 0x00010105
								 * nop out the original relative 900a0028 load
								 * */
								int registerLoad28 = Integer.parseInt(line.split("\tldr\t")[1].split(",")[0].substring(1)); // cut off the "r"
								String patchTargetLoad28 = line.split(":")[0];
								
								// step 1 from above
								// construct replacement ldr instruction
								// I wrote 0.01% of an ARM assembler, yay!
								long newLdrInstruction = ldr_Rx_pc_Y_mask;
								newLdrInstruction |= (registerLoad28 << 12);
								newLdrInstruction |= pcOffset900a0000;
								String hexLdr = String.format("%16X", newLdrInstruction).substring(8);
								System.out.println("\tput_word(0x"+patchTargetLoad900a0000.toUpperCase()+", 0x"+hexLdr+");");
								
								// step 2 from above
								System.out.println("\tput_word(0x"+addrLoad900a0000.toUpperCase()+", "+replace900a0028+");");
								
								// step 3 from above
								System.out.println("\tput_word(0x"+patchTargetLoad28.toUpperCase()+", NOP);");
							} else if(line.contains(str2c)) {
								found2c = true;
								// way easier, just replace ldr with mov [reg], #0x04000001
								int registerLoad2c = Integer.parseInt(line.split("\tldr\t")[1].split(",")[0].substring(1)); // cut off the "r"
								String patchTargetLoad2c = line.split(":")[0];
								// mov rX,#0x04000001 = E3A0x341
								long newMovInstruction = mov_Rx_0x04000001_mask;
								newMovInstruction |= (registerLoad2c << 12);
								String hexReplacementLoad2c = String.format("%16X", newMovInstruction).substring(8);
								System.out.println("\tput_word(0x"+patchTargetLoad2c.toUpperCase()+", 0x"+hexReplacementLoad2c+");");
							}
						}
					}
					while(sc.hasNextLine() // there is more objdump to process
						  && !(line.contains("; 0x") && addrs.contains(";"+line.split(";")[1])) // and ANOTHER addr hasn't appeared (fix for VERY hard to find bug)
						  && !line.contains(ldrit) && !line.contains(movit) // and the 900a0000 register hasn't been written with anything else
						  && !(found28 && found2c)); // and it hasn't found both 28 & 2c references
				}
			}
		}
		sc.close();
	}
}
