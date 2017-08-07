import java.io.*;
import java.util.*;
import java.nio.*;
import java.nio.file.*;

public class FindStrOffset {
	public static int indexOf(byte[] outerArray, byte[] smallerArray) {
		for(int i = 0; i < outerArray.length - smallerArray.length+1; ++i) {
			boolean found = true;
			for(int j = 0; j < smallerArray.length; ++j) {
				if (outerArray[i+j] != smallerArray[j]) {
					found = false;
					break;
				}
			}
			if (found) return i;
		}
		return -1;
	}
	
	public static void main(String args[]) throws Exception {
		String in = args[0];
		String fstr = args[1];
		byte[] bytes = Files.readAllBytes(Paths.get(in));
		String text = new String(bytes);
		int w = indexOf(bytes,fstr.getBytes());
		System.out.println("\twrite_osfilename(0x"+Integer.toString(w+0x10000000,16)+");");
	}
}
