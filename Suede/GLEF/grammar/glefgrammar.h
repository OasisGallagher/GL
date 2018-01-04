#pragma once

const char* const GLEFGrammar =
"$Program\n"
"	: $Blocks		$$ = $1\n"
"\n"
"$Blocks\n"
"	: $PropertyBlock $SubShaders		$$ = make(\"Blocks\", $2, $1)\n"
"\n"
"$PropertyBlock\n"
"	: Properties { $Properties }			$$ = make(\"PropertyBlock\", $2)\n"
"\n"
"$Properties\n"
"	: epsilon				$$ = $0\n"
"	| $PropertiesPrefix		$$ = $1\n"
"\n"
"$PropertiesPrefix\n"
"	: $PropertiesPrefix $Property			$$ = make(\"Properties\", $2, $1)\n"
"	| $Property								$$ = $1\n"
"\n"
"$Property\n"
"	: int $Identifier $IntegerInitializer ;		$$ = make(\"Int\", $3, $2)\n"
"	| float $Identifier $SingleInitializer ;	$$ = make(\"Single\", $3, $2)\n"
"	| vec3 $Identifier $Vec3Initializer ;		$$ = make(\"Vec3\", $3, $2)\n"
"	| vec4 $Identifier $Vec4Initializer ;		$$ = make(\"Vec4\", $3, $2)\n"
"	| tex2 $Identifier $IVec3Initializer ;		$$ = make(\"Tex2\", $3, $2)\n"
"	| mat3 $Identifier ;						$$ = make(\"Mat3\", $2)\n"
"	| mat4 $Identifier ;						$$ = make(\"Mat4\", $2)\n"
"\n"
"$IntegerInitializer\n"
"	: epsilon		$$ = $0\n"
"	| = $Integer	$$ = $1\n"
"\n"
"$SingleInitializer\n"
"	: epsilon		$$ = $0\n"
"	| = $Integer	$$ = $1\n"
"	| = $Single		$$ = $1\n"
"\n"
"$Vec3Initializer\n"
"	: epsilon			$$ = $0\n"
"	| = { $Number3 }	$$ = $2\n"
"\n"
"$IVec3Initializer\n"
"	: epsilon			$$ = $0\n"
"	| = { $Integers3 }	$$ = $2\n"
"\n"
"$Vec4Initializer\n"
"	: epsilon			$$ = $0\n"
"	| = { $Number4 }	$$ = $2\n"
"\n"
"$Integers3\n"
"	: epsilon							$$ = $0\n"
"	| $Integer							$$ = make(\"Integer3\", $1, $0, $0)\n"
"	| $Integer , $Integer				$$ = make(\"Integer3\", $3, $1, $0)\n"
"	| $Integer , $Integer , $Integer	$$ = make(\"Integer3\", $5, $3, $1)\n"
"\n"
"$Number\n"
"	: $Integer						$$ = $1\n"
"	| $Single						$$ = $1\n"
"\n"
"$Number3\n"
"	: epsilon						$$ = $0\n"
"	| $Number						$$ = make(\"Singles3\", $1, $0, $0)\n"
"	| $Number , $Number				$$ = make(\"Singles3\", $3, $1, $0)\n"
"	| $Number , $Number , $Number	$$ = make(\"Singles3\", $5, $3, $1)\n"
"\n"
"$Number4\n"
"	: epsilon									$$ = $0\n"
"	| $Number									$$ = make(\"Singles4\", $1, $0, $0, $0)\n"
"	| $Number , $Number							$$ = make(\"Singles4\", $3, $1, $0, $0)\n"
"	| $Number , $Number , $Number				$$ = make(\"Singles4\", $5, $3, $1, $0)\n"
"	| $Number , $Number , $Number , $Number		$$ = make(\"Singles4\", $7, $5, $3, $1)\n"
"\n"
"$SubShaders\n"
"	: $SubShadersPrefix			$$ = $1\n"
"\n"
"$SubShadersPrefix\n"
"	: $SubShadersPrefix $SubShader		$$ = make(\"SubShaders\", $2, $1)\n"
"	| $SubShader						$$ = $1\n"
"\n"
"$SubShader\n"
"	: SubShader { $TagBlock $PassesPrefix }		$$ = make(\"SubShader\", $3, $2)\n"
"\n"
"$TagBlock\n"
"	: epsilon			$$ = $0\n"
"	| Tags { $Tags }	$$ = make(\"TagBlock\", $2)\n"
"\n"
"$Tags\n"
"	: epsilon		$$ = $0\n"
"	| $TagsPrefix	$$ = $1\n"
"\n"
"$TagsPrefix\n"
"	: $TagsPrefix $Tag	$$ = make(\"Tags\", $2, $1)\n"
"	| $Tag				$$ = $1\n"
"\n"
"$Tag\n"
"	: Queue = $String ;	$$ = make(\"Queue\", $2)\n"
"\n"
"$PassesPrefix\n"
"	: $PassesPrefix $Pass		$$ = make(\"Passes\", $2, $1)\n"
"	| $Pass						$$ = $1\n"
"\n"
"$Pass\n"
"	: Pass { $RenderStates $ShaderCode } 		$$ = make(\"Pass\", $3, $2)\n"
"\n"
"$ShaderCode\n"
"	: epsilon			$$ = $0\n"
"	| $Code				$$ = $1\n"
"\n"
"$RenderStates\n"
"	: epsilon				$$ = $0\n"
"	| $RenderStatesPrefix	$$ = $1\n"
"\n"
"$RenderStatesPrefix\n"
"	: $RenderStatesPrefix $RenderState	$$ = make(\"RenderStates\", $2, $1)\n"
"	| $RenderState						$$ = $1\n"
"\n"
"$RenderState\n"
"	: $Identifier $RSP ;					$$ = make(\"RenderState\", $3, $2, $0, $0)\n"
"	| $Identifier $RSP $RSP ;				$$ = make(\"RenderState\", $4, $3, $2, $0)\n"
"	| $Identifier $RSP $RSP $RSP ;			$$ = make(\"RenderState\", $5, $4, $3, $2)\n"
"\n"
"$RSP\n"
"	: $Identifier		$$ = $1\n"
"	| $Integer			$$ = $1\n"
"\n"
"$Integer\n"
"	: integer		$$ = integer($1)\n"
"\n"
"$Single\n"
"	: single		$$ = single($1)\n"
"\n"
"$Code\n"
"	: code			$$ = code($1)\n"
"\n"
"$Identifier\n"
"	: identifier	$$ = symbol($1)\n"
"\n"
"$String\n"
"	: string		$$ = literal($1)";
