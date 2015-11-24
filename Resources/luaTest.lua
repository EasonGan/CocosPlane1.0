--攻击公式
function RoleAtkFormula( grade, coe1, coe2, coe3, coe4 )						
	return coe1 * grade + coe2 * coe3 * coe4
end

--武器数公式
function RoleWeaponNumFormula( grade, bass, coe )							
	return bass + grade/10 
end

--防御公式
function RoleDefenseFormula( grade, bass, coe )								
	return coe * grade + bass
end

--幸运公式
function RoleLuckFormula( grade, coe )										
	return coe * grade 
end

--经验公式
function RoleExpFormula( grade, bass, coe1, coe2, coe3 )						
	return coe1 * grade * grade + coe3 * grade
end

--HP公式
function RoleHpFormula( grade, bass, coe1, coe2, coe3, coe4 )				
	return coe1 * grade + bass + pow(coe2 , coe3) * coe4 
end