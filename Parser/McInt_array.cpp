#include "McFunction.h"

std::string McFunction::GetArrayName(std::string a) {
	std::string Out = "";
	Out = a.substr(0,(int)(a.find('[')));
	return Out;
}
std::string McFunction::GetArrayIndex(std::string a) {
	
	std::string Out = "";
	int tmp = 0; 
	tmp = (int)(a.find('[')+1);
	Out = a.substr(tmp);
	tmp = Out.length()-1;
	Out = Out.substr(0,tmp);
	//Out = a.substr((int)(a.find('['))+1, a.size()-2);
	
	return Out;
}



std::string McFunction::McCreateNewIntArray(std::string a) {
	std::string Out = "";
	
	if(!this->IntArrayIndex) {
		this->IntArrayIndex = true;
		Out += McAddIntTab("SMPL_ArrayIdex");
		Out += "scoreboard players set MaxArraySize SMPL_ArrayIdex 0\n";
	}
	if(!this->baseIntTabs) {
		this->baseIntTabs = true;
		Out += McAddIntTab("SMPL_Int");
		Out += McAddIntTab("SMPL_Tmp");
	}
	if(!this->IntArrayPointer) {
		this->IntArrayPointer = true;
		Out += "execute positioned 0 0 0 run summon minecraft:armor_stand ~ ~ ~ {Marker:1,Tags:[\"SMPL_ArrayPointer\"],Invisible:1}\n";
		
		
	}
	
	std::string TabName = GetArrayName(a);
	Out += McAddIntTab("SMPL_Array"+TabName);
	

	std::string stringTabSize = GetArrayIndex(a);
	
	
	
	if(isNumber(stringTabSize)) {
		int TabSize = std::stoi(stringTabSize);
		Out += "scoreboard players set ArraySize SMPL_Array" + TabName  + " " + std::to_string(TabSize) + "\n";
		if(MaxArraySize<TabSize) {
			for(int i = MaxArraySize; i < TabSize; i++) {
				Out += "execute positioned 0 0 0 run summon minecraft:armor_stand ~ ~ ~ {Marker:1,Tags:[\"SMPL_ArrayElement\"],Invisible:1}\n";
				Out += "execute as @e[type=minecraft:armor_stand,tag=SMPL_ArrayElement] unless score @s SMPL_ArrayIdex matches 0.. run scoreboard players set @s SMPL_ArrayIdex " + std::to_string(i) + "\n";
			}
			MaxArraySize = TabSize;
			Out += "scoreboard players set MaxArraySize SMPL_ArrayIdex " + std::to_string(MaxArraySize) + "\n";
		}
		for(int i = 0; i < TabSize; i++) {
			Out += "execute as @e[type=minecraft:armor_stand,tag=SMPL_ArrayElement] unless score @s SMPL_ArrayIdex matches " + std::to_string(i) + " run scoreboard players set @s SMPL_Array" + TabName + " " + std::to_string(0) + "\n";
		}
	}else {
		
	}
	
	return Out;
}
std::string McFunction::McGetValueFromIntArray(std::string array, std::string index) {
	std::string Out = "";
	std::string tmpIndex = index;
	if(isNumber(index)) {
		Out += "scoreboard players set <arrayIndexTmp> SMPL_Tmp " + index + "\n";
		index = "<arrayIndexTmp> SMPL_Tmp";
	}else {
		index += " SMPL_Int";
	}
	Out += "scoreboard players operation @e[type=minecraft:armor_stand,tag=SMPL_ArrayPointer] SMPL_ArrayIdex = " + index + "\n"; 
	Out += "execute as @e[type=minecraft:armor_stand,tag=SMPL_ArrayElement] if score @s SMPL_ArrayIdex = @e[type=minecraft:armor_stand,tag=SMPL_ArrayPointer,limit=1] SMPL_ArrayIdex run scoreboard players operation <array" + array + tmpIndex + "> SMPL_Tmp = @s SMPL_Array" + array + "\n";
	
	return Out;
}
std::string McFunction::McSetValueOnIntArray(std::string array, std::string index, std::string value) {
	//Value contains soreboard palyer and scoreboard
	std::string Out = "";
	std::string tmpIndex = index;
	if(isNumber(index)) {
		Out += "scoreboard players set <arrayIndexTmp> SMPL_Tmp " + index + "\n";
		index = "<arrayIndexTmp> SMPL_Tmp";
	}else {
		index += " SMPL_Int";
	}
	Out += "scoreboard players operation @e[type=minecraft:armor_stand,tag=SMPL_ArrayPointer,limit=1] SMPL_ArrayIdex = " + index + "\n"; 
	
	if(isNumber(value)) {
		//McOperationInt make sure to make value as scroeboard;
	}else {
		Out += "execute as @e[type=minecraft:armor_stand,tag=SMPL_ArrayElement] if score @s SMPL_ArrayIdex = @e[type=minecraft:armor_stand,tag=SMPL_ArrayPointer,limit=1] SMPL_ArrayIdex run scoreboard players operation @s SMPL_Array" + array + " = "+ value + "\n";
	}

	return Out;
}
std::string McFunction::McPushValueToIntArray(std::string array, std::string value) {
	
	
	std::string Out = "";
	std::string FullArrayName = "SMPL_Array"+array;
	
	if(!(isNumber(value))) {
		if(value[0] == '<' && value[value.length()-1] == '>') {
			value += " SMPL_Tmp";
		}else {
			value += " SMPL_Int";
		}
	}
	
	
	Out += "execute if score MaxArraySize SMPL_ArrayIdex = ArraySize " + FullArrayName + " positioned 0 0 0 run summon minecraft:armor_stand ~ ~ ~ {Marker:1,Tags:[\"SMPL_ArrayElement\"],Invisible:1}\n";
	Out += "execute if score MaxArraySize SMPL_ArrayIdex = ArraySize " + FullArrayName + " as @e[type=minecraft:armor_stand,tag=SMPL_ArrayElement] unless score @s SMPL_ArrayIdex matches 0.. run scoreboard players operation @s SMPL_ArrayIdex = MaxArraySize SMPL_ArrayIdex\n";
	Out += "execute if score MaxArraySize SMPL_ArrayIdex = ArraySize " + FullArrayName + " run scoreboard players add MaxArraySize SMPL_ArrayIdex 1\n";
		
	Out += "execute if score MaxArraySize SMPL_ArrayIdex > ArraySize " + FullArrayName + " as @e[type=minecraft:armor_stand,tag=SMPL_ArrayElement] unless score @s SMPL_ArrayIdex = ArraySize " + FullArrayName + " run scoreboard players operation @s " + FullArrayName + " = "+ value + "\n";
	Out += "execute if score MaxArraySize SMPL_ArrayIdex > ArraySize " + FullArrayName + " run scoreboard players add ArraySize " + FullArrayName + " 1\n";
	return Out;
}

std::string McFunction::McExpandIntArrayByOne(std::string array) {
	std::string Out = "";
	std::string FullArrayName = "SMPL_Array"+array;
	
	Out += "execute if score MaxArraySize SMPL_ArrayIdex = ArraySize " + FullArrayName + " positioned 0 0 0 run summon minecraft:armor_stand ~ ~ ~ {Marker:1,Tags:[\"SMPL_ArrayElement\"],Invisible:1}\n";
	Out += "execute if score MaxArraySize SMPL_ArrayIdex = ArraySize " + FullArrayName + " as @e[type=minecraft:armor_stand,tag=SMPL_ArrayElement] unless score @s SMPL_ArrayIdex matches 0.. run scoreboard players operation @s SMPL_ArrayIdex = MaxArraySize SMPL_ArrayIdex\n";
	Out += "execute if score MaxArraySize SMPL_ArrayIdex = ArraySize " + FullArrayName + " run scoreboard players add MaxArraySize SMPL_ArrayIdex 1\n";
	
	Out += "execute if score MaxArraySize SMPL_ArrayIdex > ArraySize " + FullArrayName + " as @e[type=minecraft:armor_stand,tag=SMPL_ArrayElement] unless score @s SMPL_ArrayIdex = ArraySize " + FullArrayName + " run scoreboard players set @s " + FullArrayName + " " + std::to_string(0) + "\n";
	Out += "execute if score MaxArraySize SMPL_ArrayIdex > ArraySize " + FullArrayName + " run scoreboard players add ArraySize " + FullArrayName + " 1\n";
	return Out;
}
