
#include "macros.shaderinc"

#define ShaderName reflectionProbeCubemapToOct

float3 ArrayOfDirections[100];
#define Parameters(X) \
	X(sampler2D, cubeTexture0)\
	X(sampler2D, cubeTexture1)\
	X(sampler2D, cubeTexture2)\
	X(sampler2D, cubeTexture3)\
	X(sampler2D, cubeTexture4)\
	X(sampler2D, cubeTexture5)\

#include "shaderMacros.cpp"
#include "shaderMacros.shaderinc"

#ifdef vertexShader
in float3 VertexPos;
in float2 VertexUV0;
out float2 PSVertexUV;
void main()
{
	gl_Position = float4(VertexPos.xy * 2.0 - 1.0, 0.0f, 1.0);
	PSVertexUV = VertexUV0;
}
#endif

#ifdef pixelShader
in float2 PSVertexUV;
out float4 FragColor;
void main()
{
	// LOL NUMBERS
	ArrayOfDirections[0]  = float3(0.241128931597171, -0.208583230315057, 0.947813206480129);
	ArrayOfDirections[1]  = float3(-0.21701810514631, 0.882619087268018, 0.416997228802393);
	ArrayOfDirections[2]  = float3(-0.312105606750263, 0.200747672920292, -0.928595962758905);
	ArrayOfDirections[3]  = float3(0.415808436691932, -0.341363946899288, 0.842955514682211);
	ArrayOfDirections[4]  = float3(0.157884662819128, -0.08883266758539, -0.983453705273489);
	ArrayOfDirections[5]  = float3(-0.223355512512079, 0.309533882269613, 0.92428409634569);
	ArrayOfDirections[6]  = float3(0.88086167185034, 0.428064105517507, 0.202098581470828);
	ArrayOfDirections[7]  = float3(-0.673959317193929, 0.736124315436013, -0.0624486268172035);
	ArrayOfDirections[8]  = float3(0.683792064168089, 0.703573972675997, -0.193422020343238);
	ArrayOfDirections[9]  = float3(0.492974545433126, 0.80120859286484, 0.339176780270327);
	ArrayOfDirections[10] = float3(-0.241831647600227, -0.943068208071363, -0.228341431072041);
	ArrayOfDirections[11] = float3(0.386529697950469, -0.451236014513139, 0.804351199295816);
	ArrayOfDirections[12] = float3(-0.916474086533475, 0.289313390894501, 0.276356672728845);
	ArrayOfDirections[13] = float3(0.857257425038491, -0.484346065946524, 0.174695722954231);
	ArrayOfDirections[14] = float3(-0.0641992600286531, -0.916777171769967, -0.394205621894505);
	ArrayOfDirections[15] = float3(0.938628400795681, -0.320901271337565, 0.126487545923215);
	ArrayOfDirections[16] = float3(0.44282349835671, -0.457680563737597, -0.770996660745013);
	ArrayOfDirections[17] = float3(0.181013611421325, -0.374271903477939, -0.909480409215729);
	ArrayOfDirections[18] = float3(0.700929175529968, -0.405083523704427, 0.587031199949444);
	ArrayOfDirections[19] = float3(0.681819089731548, 0.32906576738347, 0.65332874543676);
	ArrayOfDirections[20] = float3(0.309699326611107, 0.861281296585601, 0.40284098010065);
	ArrayOfDirections[21] = float3(-0.363502189849793, 0.848969208095053, -0.383558915527801);
	ArrayOfDirections[22] = float3(-0.839487547658852, -0.512087568336394, -0.181733265202181);
	ArrayOfDirections[23] = float3(-0.993695345661743, 0.110956609045323, -0.0160683203649851);
	ArrayOfDirections[24] = float3(-0.053239071830753, -0.4714153666425, 0.880302875903469);
	ArrayOfDirections[25] = float3(0.601114339366621, -0.460102018820204, -0.65342764196612);
	ArrayOfDirections[26] = float3(0.150167621876568, 0.475316113160033, -0.866904999357124);
	ArrayOfDirections[27] = float3(-0.242667944223722, 0.945025853454324, -0.21917665283775);
	ArrayOfDirections[28] = float3(-0.909158089298808, 0.407819033993862, -0.0843516696625165);
	ArrayOfDirections[29] = float3(-0.320734706578039, 0.472958041629829, 0.820633864067268);
	ArrayOfDirections[30] = float3(-0.540172636123447, 0.717463589745559, 0.43984033531824);
	ArrayOfDirections[31] = float3(0.0475603230085104, -0.202566576348284, 0.978112875808238);
	ArrayOfDirections[32] = float3(0.78687781396403, -0.386251269261305, -0.481282934338234);
	ArrayOfDirections[33] = float3(0.177973313645895, -0.963220718464007, 0.201323985534713);
	ArrayOfDirections[34] = float3(-0.929565757878316, -0.0320671797102806, -0.367259033606447);
	ArrayOfDirections[35] = float3(0.244072658623295, 0.118831209099016, 0.962448793991983);
	ArrayOfDirections[36] = float3(-0.11228261931164, -0.96853156716279, -0.2221243273703);
	ArrayOfDirections[37] = float3(-0.579991498536504, -0.801039343228791, -0.148141257673097);
	ArrayOfDirections[38] = float3(0.565352164333151, -0.236190742568696, 0.790310611980296);
	ArrayOfDirections[39] = float3(0.480719223374878, 0.694656620667594, -0.535127281719529);
	ArrayOfDirections[40] = float3(-0.682901868432783, 0.649711890718159, 0.333945350544739);
	ArrayOfDirections[41] = float3(-0.083184863157278, -0.10793366380206, -0.990671793663153);
	ArrayOfDirections[42] = float3(0.817860999624371, 0.325917372925841, 0.474216460404468);
	ArrayOfDirections[43] = float3(-0.662444301168668, 0.0429681617673746, 0.74787785428069);
	ArrayOfDirections[44] = float3(-0.315198836795926, -0.858244806261087, 0.405050053460512);
	ArrayOfDirections[45] = float3(0.182140994916538, -0.86652649809935, -0.464711185643826);
	ArrayOfDirections[46] = float3(-0.654975447222076, 0.747277228927498, -0.112178013276577);
	ArrayOfDirections[47] = float3(0.0113275688625278, 0.999415726992303, 0.0322473382484781);
	ArrayOfDirections[48] = float3(-0.733384361701574, 0.605185033397799, -0.309674754158101);
	ArrayOfDirections[49] = float3(-0.541900512324258, 0.41054231244315, -0.733347696823641);
	ArrayOfDirections[50] = float3(0.659506218645148, -0.737642771874902, -0.14468824647873);
	ArrayOfDirections[51] = float3(0.484500666754558, 0.0775488707125232, 0.871346817613745);
	ArrayOfDirections[52] = float3(0.308339766864223, -0.434348558477175, -0.846326129762585);
	ArrayOfDirections[53] = float3(-0.3308028305936, 0.872322091741172, 0.360032853405862);
	ArrayOfDirections[54] = float3(0.844620688462415, 0.515008562544641, 0.146226102755214);
	ArrayOfDirections[55] = float3(-0.599600233145209, -0.788274144130441, 0.138215173217821);
	ArrayOfDirections[56] = float3(0.0582191900184459, -0.19500329429937, 0.979073154123832);
	ArrayOfDirections[57] = float3(-0.6009394451247, -0.21145519162814, 0.770816765014049);
	ArrayOfDirections[58] = float3(-0.797753164921074, 0.456604687349792, -0.393829972638711);
	ArrayOfDirections[59] = float3(-0.447861727730315, -0.29236862828245, 0.844949973685221);
	ArrayOfDirections[60] = float3(-0.153338237945141, -0.905200644969779, -0.396357385613256);
	ArrayOfDirections[61] = float3(-0.228694261512322, -0.0284034685331534, 0.973083849278478);
	ArrayOfDirections[62] = float3(-0.343244819236295, 0.936526757982887, 0.0714186645738077);
	ArrayOfDirections[63] = float3(0.737902640530769, 0.101985612364515, -0.667157123898376);
	ArrayOfDirections[64] = float3(0.648672666617839, -0.639656257174699, -0.412387735317331);
	ArrayOfDirections[65] = float3(0.437137957957185, -0.14354989255307, -0.887864761132586);
	ArrayOfDirections[66] = float3(0.343547642095878, 0.882154516724384, 0.322146591217619);
	ArrayOfDirections[67] = float3(0.85026239375344, 0.485303123083124, 0.20380073722741);
	ArrayOfDirections[68] = float3(-0.108937748333885, 0.984938718248677, -0.13426946146684);
	ArrayOfDirections[69] = float3(-0.965165496863261, -0.261476548727941, 0.00925084482734238);
	ArrayOfDirections[70] = float3(0.649428089464877, -0.114171141056092, 0.751803237000181);
	ArrayOfDirections[71] = float3(0.390302085255332, -0.900045302320731, 0.19386267308516);
	ArrayOfDirections[72] = float3(0.604081141243654, 0.75149217828216, -0.265227224798851);
	ArrayOfDirections[73] = float3(-0.503268863594384, -0.300643615412868, -0.810144349759904);
	ArrayOfDirections[74] = float3(0.197605774056682, 0.888575055510928, 0.413988319621714);
	ArrayOfDirections[75] = float3(-0.747797915650436, -0.229481682773368, -0.623005966761448);
	ArrayOfDirections[76] = float3(-0.644715541173872, -0.731587176579789, 0.22163500182258);
	ArrayOfDirections[77] = float3(-0.610692851202084, -0.771644378278147, -0.177817870199825);
	ArrayOfDirections[78] = float3(0.774260518513429, 0.523337595653103, -0.355862909625807);
	ArrayOfDirections[79] = float3(-0.979377738831042, 0.0738809849692711, 0.188044794509624);
	ArrayOfDirections[80] = float3(0.20885374317121, 0.516639202675757, 0.830339718562187);
	ArrayOfDirections[81] = float3(0.739453443820779, -0.609601038294868, 0.285648697758993);
	ArrayOfDirections[82] = float3(0.561098070264802, 0.644683228867953, 0.51918444695648);
	ArrayOfDirections[83] = float3(-0.772800982085629, -0.0354280248301895, 0.633658817617272);
	ArrayOfDirections[84] = float3(0.259618942707639, -0.0886732738752077, 0.961631454917954);
	ArrayOfDirections[85] = float3(0.103404767681529, 0.608381377368327, -0.786879631006005);
	ArrayOfDirections[86] = float3(0.605930848750628, 0.785533629888474, -0.125637266949663);
	ArrayOfDirections[87] = float3(-0.224975264535991, 0.206532002341905, -0.952224061004344);
	ArrayOfDirections[88] = float3(0.950030738370738, 0.235632547898858, -0.204741052359908);
	ArrayOfDirections[89] = float3(0.64979454780237, -0.717222849841785, -0.251711003555947);
	ArrayOfDirections[90] = float3(0.531939089132974, -0.248374978454669, 0.809537321888265);
	ArrayOfDirections[91] = float3(-0.723593471612087, -0.35982342819411, -0.589015779383716);
	ArrayOfDirections[92] = float3(0.123051878700225, -0.886605466278308, 0.445857580751712);
	ArrayOfDirections[93] = float3(-0.243730037753074, 0.808842613396757, 0.535134838569112);
	ArrayOfDirections[94] = float3(-0.758317220063813, 0.503145324784428, 0.414487365190138);
	ArrayOfDirections[95] = float3(-0.563465827461173, 0.524972970118506, -0.637894695015133);
	ArrayOfDirections[96] = float3(-0.0384607954528527, -0.926117523733209, 0.375269369183528);
	ArrayOfDirections[97] = float3(0.223968969510973, -0.508702665660054, -0.831299884907095);
	ArrayOfDirections[98] = float3(-0.731560482603851, 0.318337850580929, -0.602893251894508);
	ArrayOfDirections[99] = float3(0.377477790340985, 0.442317420672974, 0.813551361112804);

	float3 L00  = float3(0.0);
	float3 L11  = float3(0.0);
	float3 L10  = float3(0.0);
	float3 L1_1 = float3(0.0);
	float3 L21  = float3(0.0);
	float3 L2_1 = float3(0.0);
	float3 L2_2 = float3(0.0);
	float3 L20  = float3(0.0);
	float3 L22  = float3(0.0);

	for (int i = 0; i < 100; i++)
	{
		float3 normal = ArrayOfDirections[i];
		float3 res = RayCubemap(normal);

		float4 sample0 = Sample(cubeTexture0, res.xy);
		float4 sample1 = Sample(cubeTexture1, res.xy);
		float4 sample2 = Sample(cubeTexture2, res.xy);
		float4 sample3 = Sample(cubeTexture3, res.xy);
		float4 sample4 = Sample(cubeTexture4, res.xy);
		float4 sample5 = Sample(cubeTexture5, res.xy);

		float3 L;
		if (res.z == 0.0) L = sample0.rgb;
		if (res.z == 1.0) L = sample1.rgb;
		if (res.z == 2.0) L = sample2.rgb;
		if (res.z == 3.0) L = sample3.rgb;
		if (res.z == 4.0) L = sample4.rgb;
		if (res.z == 5.0) L = sample5.rgb;

		float Y00  = 0.282095;
		float Y11  = 0.488603 *  normal.x;
		float Y10  = 0.488603 *  normal.z;
		float Y1_1 = 0.488603 *  normal.y;
		float Y21  = 1.092548 *  normal.x * normal.z;
		float Y2_1 = 1.092548 *  normal.y * normal.z;
		float Y2_2 = 1.092548 *  normal.y * normal.x;
		float Y20  = 0.946176 *  normal.z * normal.z - 0.315392;
		float Y22  = 0.546274 * (normal.x * normal.x - normal.y * normal.y);

		L00  += L * Y00;
		L11  += L * Y11;
		L10  += L * Y10;
		L1_1 += L * Y1_1;
		L21  += L * Y21;
		L2_1 += L * Y2_1;
		L2_2 += L * Y2_2;
		L20  += L * Y20;
		L22  += L * Y22;
	}

	L00	 /= 100.0;
	L11	 /= 100.0;
	L10	 /= 100.0;
	L1_1 /= 100.0;
	L21	 /= 100.0;
	L2_1 /= 100.0;
	L2_2 /= 100.0;
	L20	 /= 100.0;
	L22	 /= 100.0;

	float height = PSVertexUV.y * 9.0f;
	float3 result = 0.0f;

	if (height < 1.0)
		FragColor = L00;
	else if (height < 2.0)
		FragColor = L11;
	else if (height < 3.0)
		FragColor = L10;
	else if (height < 4.0)
		FragColor = L1_1;
	else if (height < 5.0)
		FragColor = L21;
	else if (height < 6.0)
		FragColor = L2_1;
	else if (height < 7.0)
		FragColor = L2_2;
	else if (height < 8.0)
		FragColor = L20;
	else if (height < 9.0)
		FragColor = L22;

}
#endif
