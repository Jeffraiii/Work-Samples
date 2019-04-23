// Upgrade NOTE: replaced 'mul(UNITY_MATRIX_MVP,*)' with 'UnityObjectToClipPos(*)'

Shader "Ciconia Studio/Effects/Sci-Fi/Force Field/Additive" {
    Properties {
        [Space(15)][Header(Main Properties)]
        [Space(10)]_Color ("Color", Color) = (0,0,0,1)
        [Space(25)]_SpecColor ("Specular Color", Color) = (1,1,1,1)
        _SpecularIntensity ("Specular Intensity", Range(0, 2)) = 0.2
        _FresnelStrength ("Fresnel Strength", Range(0, 8)) = 0.5
        _Glossiness ("Glossiness", Range(0, 2)) = 0.5
        _Ambientlight ("Ambient light", Range(0, 8)) = 0
        [Space(25)]_BumpMap ("Normal map", 2D) = "bump" {}
        _NormalIntensity1 ("Normal Intensity1", Range(0, 2)) = 1
        _NormalIntensity2 ("Normal Intensity2", Range(0, 2)) = 1
        [Space(15)]_AnimationSpeed1 ("Animation Speed1", Range(0, 1)) = 0.05
        _Angle1 ("Angle1", Float ) = 0
        _AnimationSpeed2 ("Animation Speed2", Range(0, 1)) = 0.05
        _Angle2 ("Angle2", Float ) = 0

        [Space(15)][Header(Additional Color)]
        [Space(10)]_WaveColorControledbynormalmap ("Wave Color (Controled by normal map)", Color) = (1,1,1,1)
        _WaveColorIntensity ("Wave Color Intensity", Range(0, 10)) = 0
       [Space(15)] _FresnelColor ("Fresnel Color", Color) = (1,1,1,1)
        _FresnelContrast ("Fresnel Contrast", Range(0, 10)) = 10
        _FresnelColorStrength ("Fresnel Color Strength", Range(0, 8)) = 1

        [Space(15)][Header(Reflection Properties)]
        [Space(10)]_Cubemap ("Cubemap", Cube) = "_Skybox" {}
        [Space(10)]_ReflectionIntensity ("Reflection Intensity", Range(0, 10)) = 2
        _BlurReflection ("Blur Reflection", Range(0, 8)) = 0

        [Space(15)][Header(Caustic Properties)]
        [Space(10)]_CausticColor ("Caustic Color", Color) = (1,1,1,1)
        _DetailMask ("Caustic Mask", 2D) = "black" {}
        [MaterialToggle] _InvertCausticMask ("Invert Mask", Float ) = 0
        [Space(10)]_CausticSpreadFresnel ("Caustic Spread(Fresnel)", Range(0, 2)) = 1
        [MaterialToggle] _node_4314 ("Invert Fresnel", Float ) = 0
        [Space(15)]_CausticAnimationSpeed1 ("Animation Speed1", Range(0, 1)) = 0.05
        _causticAngle1 ("Angle1", Float ) = 0
        _CausticAnimationSpeed2 ("Animation Speed2", Range(0, 1)) = 0.05
        _CausticAngle2 ("Angle2", Float ) = 0
        [Space(15)]_Distortion ("Distortion", Range(0, 1)) = 0
        _DistortionSpeed01 ("Distortion Speed(0.1)", Float ) = 0.1

        [Space(15)][Header(Linear Dissolve)]
        [Space(10)]_LineSpread ("Line Spread", Range(-1, 0.9)) = 0
        [Space(10)]_WaveMaskDistortionMask ("Wave Mask/Distortion Mask", 2D) = "white" {}
        [Space(10)][MaterialToggle] _SwitchYtoZaxis ("Switch Y to Z axis", Float ) = 0
        [MaterialToggle] _InvertAxis ("Invert Axis", Float ) = 1
        [Space(10)]_LinearDissolve ("Linear Dissolve", Float ) = 1
        [Space(10)]_WaveAmout ("Wave Amout", Float ) = 5
        _WaveAmplitude12 ("Wave Amplitude(12)", Float ) = 12
        _WaveSpeed ("Wave Speed", Float ) = 1
        _SmoothEdge ("Smooth Edge", Range(0, 8)) = 2

        [Space(15)][Header(Custom Dissolve)]
        [Space(10)][MaterialToggle] _InvertMask ("Invert Mask", Float ) = 0
        _TexAssetMAsk ("Dissolve Mask (Triplanar Projection)", 2D) = "white" {}
        [Space(15)]_MaskAmount ("Mask Amount", Float ) = 1
        _Contrast ("Contrast", Float ) = 1
        [Space(10)]_SmoothMask ("Smooth Mask", Range(0, 8)) = 0

        [Space(15)][Header(General Dissolve Properties)]
        [Space(10)]_EdgeColor ("Edge Color", Color) = (1,0.1882353,0,1)
        _EdgeIntensity ("Edge Intensity", Range(0, 1)) = 0.15
        [Space(15)]_DistortionAmount ("Distortion Amount", Range(0, 4)) = 0.5
        _Multiplicator ("Multiplicator", Float ) = 1
        _Speed ("Speed", Range(0, 1)) = 0.1
        [HideInInspector]_Cutoff ("Alpha cutoff", Range(0,1)) = 0.5
    }
    SubShader {
        Tags {
            "Queue"="Transparent"
            "RenderType"="Transparent"
        }
        Pass {
            Name "FORWARD"
            Tags {
                "LightMode"="ForwardBase"
            }
            Blend One One
            
            
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #define UNITY_PASS_FORWARDBASE
            #define SHOULD_SAMPLE_SH ( defined (LIGHTMAP_OFF) && defined(DYNAMICLIGHTMAP_OFF) )
            #define _GLOSSYENV 1
            #include "UnityCG.cginc"
            #include "AutoLight.cginc"
            #include "Lighting.cginc"
            #include "UnityPBSLighting.cginc"
            #include "UnityStandardBRDF.cginc"
            #pragma multi_compile_fwdbase_fullshadows
            #pragma multi_compile LIGHTMAP_OFF LIGHTMAP_ON
            #pragma multi_compile DIRLIGHTMAP_OFF DIRLIGHTMAP_COMBINED DIRLIGHTMAP_SEPARATE
            #pragma multi_compile DYNAMICLIGHTMAP_OFF DYNAMICLIGHTMAP_ON
            #pragma multi_compile_fog
            #pragma only_renderers d3d9 d3d11 glcore gles gles3 metal d3d11_9x xboxone ps4 psp2 n3ds wiiu 
            #pragma target 3.0
            uniform float4 _TimeEditor;
            uniform float4 _Color;
            uniform float _SpecularIntensity;
            uniform float _Glossiness;
            uniform float _AnimationSpeed2;
            uniform float _AnimationSpeed1;
            uniform float _NormalIntensity1;
            uniform sampler2D _BumpMap; uniform float4 _BumpMap_ST;
            uniform float _Ambientlight;
            uniform float _ReflectionIntensity;
            uniform float _BlurReflection;
            uniform float _FresnelStrength;
            uniform samplerCUBE _Cubemap;
            uniform float4 _WaveColorControledbynormalmap;
            uniform float _FresnelColorStrength;
            uniform float4 _FresnelColor;
            uniform float _CausticAnimationSpeed2;
            uniform float _CausticAnimationSpeed1;
            uniform sampler2D _DetailMask; uniform float4 _DetailMask_ST;
            uniform float _Distortion;
            uniform float _CausticSpreadFresnel;
            uniform fixed _node_4314;
            uniform fixed _InvertCausticMask;
            uniform float _WaveColorIntensity;
            uniform float4 _CausticColor;
            uniform float _FresnelContrast;
            uniform float _NormalIntensity2;
            uniform fixed _SwitchYtoZaxis;
            uniform fixed _InvertAxis;
            uniform float _LinearDissolve;
            uniform float _WaveAmout;
            uniform float _WaveAmplitude12;
            uniform float _WaveSpeed;
            uniform float _Contrast;
            uniform float _MaskAmount;
            uniform fixed _InvertMask;
            uniform sampler2D _TexAssetMAsk; uniform float4 _TexAssetMAsk_ST;
            uniform float4 _EdgeColor;
            uniform float _DistortionAmount;
            uniform float _EdgeIntensity;
            uniform float _Multiplicator;
            uniform float _Speed;
            uniform float _LineSpread;
            uniform float _SmoothEdge;
            uniform sampler2D _WaveMaskDistortionMask; uniform float4 _WaveMaskDistortionMask_ST;
            uniform float _DistortionSpeed01;
            uniform float _SmoothMask;
            uniform float _CausticAngle2;
            uniform float _causticAngle1;
            uniform float _Angle1;
            uniform float _Angle2;
            struct VertexInput {
                float4 vertex : POSITION;
                float3 normal : NORMAL;
                float4 tangent : TANGENT;
                float2 texcoord0 : TEXCOORD0;
                float2 texcoord1 : TEXCOORD1;
                float2 texcoord2 : TEXCOORD2;
            };
            struct VertexOutput {
                float4 pos : SV_POSITION;
                float2 uv0 : TEXCOORD0;
                float2 uv1 : TEXCOORD1;
                float2 uv2 : TEXCOORD2;
                float4 posWorld : TEXCOORD3;
                float3 normalDir : TEXCOORD4;
                float3 tangentDir : TEXCOORD5;
                float3 bitangentDir : TEXCOORD6;
                LIGHTING_COORDS(7,8)
                UNITY_FOG_COORDS(9)
                #if defined(LIGHTMAP_ON) || defined(UNITY_SHOULD_SAMPLE_SH)
                    float4 ambientOrLightmapUV : TEXCOORD10;
                #endif
            };
            VertexOutput vert (VertexInput v) {
                VertexOutput o = (VertexOutput)0;
                o.uv0 = v.texcoord0;
                o.uv1 = v.texcoord1;
                o.uv2 = v.texcoord2;
                #ifdef LIGHTMAP_ON
                    o.ambientOrLightmapUV.xy = v.texcoord1.xy * unity_LightmapST.xy + unity_LightmapST.zw;
                    o.ambientOrLightmapUV.zw = 0;
                #endif
                #ifdef DYNAMICLIGHTMAP_ON
                    o.ambientOrLightmapUV.zw = v.texcoord2.xy * unity_DynamicLightmapST.xy + unity_DynamicLightmapST.zw;
                #endif
                o.normalDir = UnityObjectToWorldNormal(v.normal);
                o.tangentDir = normalize( mul( unity_ObjectToWorld, float4( v.tangent.xyz, 0.0 ) ).xyz );
                o.bitangentDir = normalize(cross(o.normalDir, o.tangentDir) * v.tangent.w);
                float4 objPos = mul ( unity_ObjectToWorld, float4(0,0,0,1) );
                o.posWorld = mul(unity_ObjectToWorld, v.vertex);
                float3 lightColor = _LightColor0.rgb;
                o.pos = UnityObjectToClipPos(v.vertex );
                UNITY_TRANSFER_FOG(o,o.pos);
                TRANSFER_VERTEX_TO_FRAGMENT(o)
                return o;
            }
            float4 frag(VertexOutput i) : COLOR {
                float4 objPos = mul ( unity_ObjectToWorld, float4(0,0,0,1) );
                i.normalDir = normalize(i.normalDir);
                float3x3 tangentTransform = float3x3( i.tangentDir, i.bitangentDir, i.normalDir);
                float3 viewDirection = normalize(_WorldSpaceCameraPos.xyz - i.posWorld.xyz);
                float node_2220_ang = ((_Angle1*3.141592654)/180.0);
                float node_2220_spd = 1.0;
                float node_2220_cos = cos(node_2220_spd*node_2220_ang);
                float node_2220_sin = sin(node_2220_spd*node_2220_ang);
                float2 node_2220_piv = float2(0.5,0.5);
                float2 node_2220 = (mul(i.uv0-node_2220_piv,float2x2( node_2220_cos, -node_2220_sin, node_2220_sin, node_2220_cos))+node_2220_piv);
                float4 node_4748 = _Time + _TimeEditor;
                float2 node_1056 = (i.uv0/4.0);
                float2 node_6038 = (node_2220+(node_1056+(node_4748.g*_AnimationSpeed1)*float2(0,0.6)));
                float3 _NormalWave1Vertical = UnpackNormal(tex2D(_BumpMap,TRANSFORM_TEX(node_6038, _BumpMap)));
                float node_8828_ang = ((_Angle2*3.141592654)/180.0);
                float node_8828_spd = 1.0;
                float node_8828_cos = cos(node_8828_spd*node_8828_ang);
                float node_8828_sin = sin(node_8828_spd*node_8828_ang);
                float2 node_8828_piv = float2(0.5,0.5);
                float2 node_8828 = (mul(i.uv0-node_8828_piv,float2x2( node_8828_cos, -node_8828_sin, node_8828_sin, node_8828_cos))+node_8828_piv);
                float4 node_7920 = _Time + _TimeEditor;
                float2 node_5862 = (node_8828+(1.0 - (node_1056+(node_7920.g*_AnimationSpeed2)*float2(0,0.6))));
                float3 _NormalWave2Horizontal = UnpackNormal(tex2D(_BumpMap,TRANSFORM_TEX(node_5862, _BumpMap)));
                float3 node_5996_nrm_base = lerp(float3(0,0,1),_NormalWave1Vertical.rgb,_NormalIntensity1) + float3(0,0,1);
                float3 node_5996_nrm_detail = lerp(float3(0,0,1),_NormalWave2Horizontal.rgb,_NormalIntensity2) * float3(-1,-1,1);
                float3 node_5996_nrm_combined = node_5996_nrm_base*dot(node_5996_nrm_base, node_5996_nrm_detail)/node_5996_nrm_base.z - node_5996_nrm_detail;
                float3 node_5996 = node_5996_nrm_combined;
                float3 Normalmap = node_5996;
                float3 normalLocal = Normalmap;
                float3 normalDirection = normalize(mul( normalLocal, tangentTransform )); // Perturbed normals
                float3 viewReflectDirection = reflect( -viewDirection, normalDirection );
                float3 NormalDirectionMask = mul( unity_WorldToObject, float4(saturate(((abs(i.normalDir)*2.0)*0.5)),0) ).xyz.rgb;
                float3 node_7209 = NormalDirectionMask;
                float3 node_9426 = mul( unity_WorldToObject, float4((i.posWorld.rgb-objPos.rgb),0) ).xyz;
                float2 GB = node_9426.rgb.gb;
                float2 node_7268 = GB;
                float4 _node_1257 = tex2Dlod(_TexAssetMAsk,float4(TRANSFORM_TEX(node_7268, _TexAssetMAsk),0.0,_SmoothMask)); // X Axis FrontBack
                float2 RB = node_9426.rgb.rb;
                float2 node_9779 = RB;
                float4 _node_4330 = tex2Dlod(_TexAssetMAsk,float4(TRANSFORM_TEX(node_9779, _TexAssetMAsk),0.0,_SmoothMask)); // Y Axis TopBottom
                float2 RG = node_9426.rgb.rg;
                float2 node_7670 = RG;
                float4 _node_3650 = tex2Dlod(_TexAssetMAsk,float4(TRANSFORM_TEX(node_7670, _TexAssetMAsk),0.0,_SmoothMask)); // Z Axis LeftRight 
                float node_4586 = (node_7209.r*(_node_1257.r*NormalDirectionMask.r) + node_7209.g*(_node_4330.r*NormalDirectionMask.g) + node_7209.b*(_node_3650.r*NormalDirectionMask.b));
                float node_9251 = 0.0;
                float node_7590 = (1.0+(-1*_Contrast));
                float node_1322 = saturate(((_MaskAmount*2.0+-1.0)+(node_7590 + ( (lerp( node_4586, (1.0 - node_4586), _InvertMask ) - node_9251) * (_Contrast - node_7590) ) / (1.0 - node_9251))));
                float MaskDisslolveCloud = node_1322;
                float CutoutDissolveCloud = MaskDisslolveCloud.r;
                float3 node_1607 = NormalDirectionMask;
                float node_5233 = 0.1;
                float2 node_2619 = (node_5233*GB);
                float4 _node_12570 = tex2Dlod(_WaveMaskDistortionMask,float4(TRANSFORM_TEX(node_2619, _WaveMaskDistortionMask),0.0,_SmoothEdge)); // X Axis FrontBack
                float2 node_8090 = (node_5233*RB);
                float4 _node_43300 = tex2Dlod(_WaveMaskDistortionMask,float4(TRANSFORM_TEX(node_8090, _WaveMaskDistortionMask),0.0,_SmoothEdge)); // Y Axis TopBottom
                float2 node_7721 = (node_5233*RG);
                float4 _node_36500 = tex2Dlod(_WaveMaskDistortionMask,float4(TRANSFORM_TEX(node_7721, _WaveMaskDistortionMask),0.0,_SmoothEdge)); // Z Axis LeftRight 
                float WaveLineMask = (node_1607.r*(_node_12570.r*NormalDirectionMask.r) + node_1607.g*(_node_43300.r*NormalDirectionMask.g) + node_1607.b*(_node_36500.r*NormalDirectionMask.b));
                float4 node_2749 = _Time + _TimeEditor;
                float3 node_9783 = mul( unity_WorldToObject, float4((i.posWorld.rgb-objPos.rgb),0) ).xyz;
                float _SwitchYtoZaxis_var = lerp( node_9783.rgb.g, node_9783.rgb.b, _SwitchYtoZaxis );
                float node_1447 = saturate(((_LinearDissolve+(sin(((_WaveAmout*WaveLineMask)+(node_2749.g*_WaveSpeed)))+((-1*_WaveAmplitude12)*(lerp( _SwitchYtoZaxis_var, (1.0 - _SwitchYtoZaxis_var), _InvertAxis )*2.0+-1.0))))*lerp(2,0,_LineSpread)));
                float Cutout = saturate((CutoutDissolveCloud*node_1447));
                clip(Cutout - 0.5);
                float3 lightDirection = normalize(_WorldSpaceLightPos0.xyz);
                float3 lightColor = _LightColor0.rgb;
                float3 halfDirection = normalize(viewDirection+lightDirection);
////// Lighting:
                float attenuation = LIGHT_ATTENUATION(i);
                float3 attenColor = attenuation * _LightColor0.xyz;
                float Pi = 3.141592654;
                float InvPi = 0.31830988618;
///////// Gloss:
                float Glossiness = _Glossiness;
                float gloss = Glossiness;
                float perceptualRoughness = 1.0 - Glossiness;
                float roughness = perceptualRoughness * perceptualRoughness;
                float specPow = exp2( gloss * 10.0 + 1.0 );
/////// GI Data:
                UnityLight light;
                #ifdef LIGHTMAP_OFF
                    light.color = lightColor;
                    light.dir = lightDirection;
                    light.ndotl = LambertTerm (normalDirection, light.dir);
                #else
                    light.color = half3(0.f, 0.f, 0.f);
                    light.ndotl = 0.0f;
                    light.dir = half3(0.f, 0.f, 0.f);
                #endif
                UnityGIInput d;
                d.light = light;
                d.worldPos = i.posWorld.xyz;
                d.worldViewDir = viewDirection;
                d.atten = attenuation;
                #if defined(LIGHTMAP_ON) || defined(DYNAMICLIGHTMAP_ON)
                    d.ambient = 0;
                    d.lightmapUV = i.ambientOrLightmapUV;
                #else
                    d.ambient = i.ambientOrLightmapUV;
                #endif
                #if UNITY_SPECCUBE_BLENDING || UNITY_SPECCUBE_BOX_PROJECTION
                    d.boxMin[0] = unity_SpecCube0_BoxMin;
                    d.boxMin[1] = unity_SpecCube1_BoxMin;
                #endif
                #if UNITY_SPECCUBE_BOX_PROJECTION
                    d.boxMax[0] = unity_SpecCube0_BoxMax;
                    d.boxMax[1] = unity_SpecCube1_BoxMax;
                    d.probePosition[0] = unity_SpecCube0_ProbePosition;
                    d.probePosition[1] = unity_SpecCube1_ProbePosition;
                #endif
                d.probeHDR[0] = unity_SpecCube0_HDR;
                d.probeHDR[1] = unity_SpecCube1_HDR;
                Unity_GlossyEnvironmentData ugls_en_data;
                ugls_en_data.roughness = 1.0 - gloss;
                ugls_en_data.reflUVW = viewReflectDirection;
                UnityGI gi = UnityGlobalIllumination(d, 1, normalDirection, ugls_en_data );
                lightDirection = gi.light.dir;
                lightColor = gi.light.color;
////// Specular:
                float NdotL = saturate(dot( normalDirection, lightDirection ));
                float4 _Cubemap_var = texCUBElod(_Cubemap,float4(viewReflectDirection,_BlurReflection));
                float3 CubemapSpec = ((((0.95*pow(1.0-max(0,dot(normalDirection, viewDirection)),1.0))+0.05)*_FresnelStrength)+(_Cubemap_var.rgb*(_Cubemap_var.a*_ReflectionIntensity)));
                float LdotH = saturate(dot(lightDirection, halfDirection));
                float3 Specular = (_SpecColor.rgb*_SpecularIntensity);
                float3 specularColor = Specular;
                float specularMonochrome;
                float3 BaseColor = _Color.rgb;
                float3 diffuseColor = BaseColor; // Need this for specular when using metallic
                diffuseColor = EnergyConservationBetweenDiffuseAndSpecular(diffuseColor, specularColor, specularMonochrome);
                specularMonochrome = 1.0-specularMonochrome;
                float NdotV = abs(dot( normalDirection, viewDirection ));
                float NdotH = saturate(dot( normalDirection, halfDirection ));
                float VdotH = saturate(dot( viewDirection, halfDirection ));
                float visTerm = SmithJointGGXVisibilityTerm( NdotL, NdotV, roughness );
                float normTerm = GGXTerm(NdotH, roughness);
                float specularPBL = (visTerm*normTerm) * UNITY_PI;
                #ifdef UNITY_COLORSPACE_GAMMA
                    specularPBL = sqrt(max(1e-4h, specularPBL));
                #endif
                specularPBL = max(0, specularPBL * NdotL);
                #if defined(_SPECULARHIGHLIGHTS_OFF)
                    specularPBL = 0.0;
                #endif
                half surfaceReduction;
                #ifdef UNITY_COLORSPACE_GAMMA
                    surfaceReduction = 1.0-0.28*roughness*perceptualRoughness;
                #else
                    surfaceReduction = 1.0/(roughness*roughness + 1.0);
                #endif
                specularPBL *= any(specularColor) ? 1.0 : 0.0;
                float3 directSpecular = attenColor*specularPBL*FresnelTerm(specularColor, LdotH);
                half grazingTerm = saturate( gloss + specularMonochrome );
                float3 indirectSpecular = (gi.indirect.specular + CubemapSpec);
                indirectSpecular *= FresnelLerp (specularColor, grazingTerm, NdotV);
                indirectSpecular *= surfaceReduction;
                float3 specular = (directSpecular + indirectSpecular);
/////// Diffuse:
                NdotL = max(0.0,dot( normalDirection, lightDirection ));
                half fd90 = 0.5 + 2 * LdotH * LdotH * (1-gloss);
                float nlPow5 = Pow5(1-NdotL);
                float nvPow5 = Pow5(1-NdotV);
                float3 directDiffuse = ((1 +(fd90 - 1)*nlPow5) * (1 + (fd90 - 1)*nvPow5) * NdotL) * attenColor;
                float3 indirectDiffuse = float3(0,0,0);
                float AmbientLight = _Ambientlight;
                float node_3764 = AmbientLight;
                indirectDiffuse += float3(node_3764,node_3764,node_3764); // Diffuse Ambient Light
                indirectDiffuse += gi.indirect.diffuse;
                diffuseColor *= 1-specularMonochrome;
                float3 diffuse = (directDiffuse + indirectDiffuse) * diffuseColor;
////// Emissive:
                float4 node_4409 = _Time + _TimeEditor;
                float2 node_2490 = (i.uv0+(node_4409.g*_Speed)*float2(0,0.1));
                float4 _Burnefect = tex2D(_WaveMaskDistortionMask,TRANSFORM_TEX(node_2490, _WaveMaskDistortionMask));
                float2 node_9496 = lerp(i.uv0,float2(_Burnefect.r,_Burnefect.r),(lerp(0,0.1,_DistortionAmount)*_Multiplicator));
                float4 _node_5086 = tex2D(_WaveMaskDistortionMask,TRANSFORM_TEX(node_9496, _WaveMaskDistortionMask));
                float EmissiveColorLine = saturate(saturate((node_1447*(1.0 - node_1447))));
                float node_1330 = EmissiveColorLine;
                float node_8041 = lerp(0.6,1,_EdgeIntensity);
                float node_2708 = 0.0;
                float node_3706 = lerp(-10,0,node_8041); // Rang min
                float3 EmissiveColor = saturate((saturate((saturate(( (node_1330+(1.0 - node_1322)) > 0.5 ? (1.0-(1.0-2.0*((node_1330+(1.0 - node_1322))-0.5))*(1.0-_EdgeColor.rgb)) : (2.0*(node_1330+(1.0 - node_1322))*_EdgeColor.rgb) ))/(1.0-_node_5086.rgb)))*(node_3706 + ( (pow((1.0 - node_1330),lerp(2,0.1,node_8041)) - node_2708) * (lerp(-7,10,node_8041) - node_3706) ) / (1.0 - node_2708))));
                float3 node_6236 = EmissiveColor;
                float WaveMaskColor = (saturate(( _NormalWave2Horizontal.r > 0.5 ? (1.0-(1.0-2.0*(_NormalWave2Horizontal.r-0.5))*(1.0-_NormalWave1Vertical.r)) : (2.0*_NormalWave2Horizontal.r*_NormalWave1Vertical.r) ))*_WaveColorIntensity);
                float4 node_6336 = _Time + _TimeEditor;
                float node_4302 = (node_6336.g*_DistortionSpeed01);
                float2 node_2872 = (i.uv0+node_4302*float2(0,0.2));
                float3 _node_2739 = UnpackNormal(tex2D(_BumpMap,TRANSFORM_TEX(node_2872, _BumpMap)));
                float Distortion = lerp(0,0.2,_Distortion);
                float node_4869_ang = ((_causticAngle1*3.141592654)/180.0);
                float node_4869_spd = 1.0;
                float node_4869_cos = cos(node_4869_spd*node_4869_ang);
                float node_4869_sin = sin(node_4869_spd*node_4869_ang);
                float2 node_4869_piv = float2(0.5,0.5);
                float2 node_4869 = (mul(i.uv0-node_4869_piv,float2x2( node_4869_cos, -node_4869_sin, node_4869_sin, node_4869_cos))+node_4869_piv);
                float4 node_8916 = _Time + _TimeEditor;
                float2 node_1526 = (i.uv0/4.0);
                float2 node_8183 = (lerp(i.uv0,float2(_node_2739.r,_node_2739.r),Distortion)+(node_4869+(node_1526+(node_8916.g*_CausticAnimationSpeed1)*float2(0,0.6))));
                float4 _Caustic1 = tex2D(_DetailMask,TRANSFORM_TEX(node_8183, _DetailMask));
                float node_5854_ang = ((_CausticAngle2*3.141592654)/180.0);
                float node_5854_spd = 1.0;
                float node_5854_cos = cos(node_5854_spd*node_5854_ang);
                float node_5854_sin = sin(node_5854_spd*node_5854_ang);
                float2 node_5854_piv = float2(0.5,0.5);
                float2 node_5854 = (mul(i.uv0-node_5854_piv,float2x2( node_5854_cos, -node_5854_sin, node_5854_sin, node_5854_cos))+node_5854_piv);
                float4 node_1805 = _Time + _TimeEditor;
                float2 node_6610 = (i.uv0+node_4302*float2(0,0.2));
                float3 _node_5242 = UnpackNormal(tex2D(_BumpMap,TRANSFORM_TEX(node_6610, _BumpMap)));
                float2 node_3743 = ((node_5854+(1.0 - (node_1526+(node_1805.g*_CausticAnimationSpeed2)*float2(0,0.6))))+lerp(i.uv0,float2(_node_5242.r,_node_5242.r),Distortion));
                float4 _Caustic2 = tex2D(_DetailMask,TRANSFORM_TEX(node_3743, _DetailMask));
                float2 node_4065 = (i.uv0*2.0+-1.0);
                float3 node_1911 = (saturate((_Caustic1.rgb*_Caustic2.rgb))*(1.0 - ((node_4065*node_4065).r*2.0+-1.0)));
                float3 node_2868 = (node_1911+node_1911);
                float node_4752 = pow(1.0-max(0,dot(normalDirection, viewDirection)),_CausticSpreadFresnel);
                float3 Caustic = (lerp( node_2868, (node_2868*-1.0+1.0), _InvertCausticMask )*lerp( node_4752, (1.0 - node_4752), _node_4314 ));
                float3 node_3814 = (((_WaveColorControledbynormalmap.rgb*WaveMaskColor)+(_CausticColor.rgb*Caustic))+saturate((((0.95*pow(1.0-max(0,dot(normalDirection, viewDirection)),_FresnelContrast))+0.05)*_FresnelColorStrength*_FresnelColor.rgb)));
                float3 EmissiveColorAndCausticFresnel = saturate((node_6236+node_3814));
                float3 emissive = EmissiveColorAndCausticFresnel;
/// Final Color:
                float3 finalColor = diffuse + specular + emissive;
                fixed4 finalRGBA = fixed4(finalColor,1);
                UNITY_APPLY_FOG(i.fogCoord, finalRGBA);
                return finalRGBA;
            }
            ENDCG
        }
        Pass {
            Name "FORWARD_DELTA"
            Tags {
                "LightMode"="ForwardAdd"
            }
            Blend One One
            
            
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #define UNITY_PASS_FORWARDADD
            #define SHOULD_SAMPLE_SH ( defined (LIGHTMAP_OFF) && defined(DYNAMICLIGHTMAP_OFF) )
            #define _GLOSSYENV 1
            #include "UnityCG.cginc"
            #include "AutoLight.cginc"
            #include "Lighting.cginc"
            #include "UnityPBSLighting.cginc"
            #include "UnityStandardBRDF.cginc"
            #pragma multi_compile_fwdadd_fullshadows
            #pragma multi_compile LIGHTMAP_OFF LIGHTMAP_ON
            #pragma multi_compile DIRLIGHTMAP_OFF DIRLIGHTMAP_COMBINED DIRLIGHTMAP_SEPARATE
            #pragma multi_compile DYNAMICLIGHTMAP_OFF DYNAMICLIGHTMAP_ON
            #pragma multi_compile_fog
            #pragma only_renderers d3d9 d3d11 glcore gles gles3 metal d3d11_9x xboxone ps4 psp2 n3ds wiiu 
            #pragma target 3.0
            uniform float4 _TimeEditor;
            uniform float4 _Color;
            uniform float _SpecularIntensity;
            uniform float _Glossiness;
            uniform float _AnimationSpeed2;
            uniform float _AnimationSpeed1;
            uniform float _NormalIntensity1;
            uniform sampler2D _BumpMap; uniform float4 _BumpMap_ST;
            uniform float4 _WaveColorControledbynormalmap;
            uniform float _FresnelColorStrength;
            uniform float4 _FresnelColor;
            uniform float _CausticAnimationSpeed2;
            uniform float _CausticAnimationSpeed1;
            uniform sampler2D _DetailMask; uniform float4 _DetailMask_ST;
            uniform float _Distortion;
            uniform float _CausticSpreadFresnel;
            uniform fixed _node_4314;
            uniform fixed _InvertCausticMask;
            uniform float _WaveColorIntensity;
            uniform float4 _CausticColor;
            uniform float _FresnelContrast;
            uniform float _NormalIntensity2;
            uniform fixed _SwitchYtoZaxis;
            uniform fixed _InvertAxis;
            uniform float _LinearDissolve;
            uniform float _WaveAmout;
            uniform float _WaveAmplitude12;
            uniform float _WaveSpeed;
            uniform float _Contrast;
            uniform float _MaskAmount;
            uniform fixed _InvertMask;
            uniform sampler2D _TexAssetMAsk; uniform float4 _TexAssetMAsk_ST;
            uniform float4 _EdgeColor;
            uniform float _DistortionAmount;
            uniform float _EdgeIntensity;
            uniform float _Multiplicator;
            uniform float _Speed;
            uniform float _LineSpread;
            uniform float _SmoothEdge;
            uniform sampler2D _WaveMaskDistortionMask; uniform float4 _WaveMaskDistortionMask_ST;
            uniform float _DistortionSpeed01;
            uniform float _SmoothMask;
            uniform float _CausticAngle2;
            uniform float _causticAngle1;
            uniform float _Angle1;
            uniform float _Angle2;
            struct VertexInput {
                float4 vertex : POSITION;
                float3 normal : NORMAL;
                float4 tangent : TANGENT;
                float2 texcoord0 : TEXCOORD0;
                float2 texcoord1 : TEXCOORD1;
                float2 texcoord2 : TEXCOORD2;
            };
            struct VertexOutput {
                float4 pos : SV_POSITION;
                float2 uv0 : TEXCOORD0;
                float2 uv1 : TEXCOORD1;
                float2 uv2 : TEXCOORD2;
                float4 posWorld : TEXCOORD3;
                float3 normalDir : TEXCOORD4;
                float3 tangentDir : TEXCOORD5;
                float3 bitangentDir : TEXCOORD6;
                LIGHTING_COORDS(7,8)
                UNITY_FOG_COORDS(9)
            };
            VertexOutput vert (VertexInput v) {
                VertexOutput o = (VertexOutput)0;
                o.uv0 = v.texcoord0;
                o.uv1 = v.texcoord1;
                o.uv2 = v.texcoord2;
                o.normalDir = UnityObjectToWorldNormal(v.normal);
                o.tangentDir = normalize( mul( unity_ObjectToWorld, float4( v.tangent.xyz, 0.0 ) ).xyz );
                o.bitangentDir = normalize(cross(o.normalDir, o.tangentDir) * v.tangent.w);
                float4 objPos = mul ( unity_ObjectToWorld, float4(0,0,0,1) );
                o.posWorld = mul(unity_ObjectToWorld, v.vertex);
                float3 lightColor = _LightColor0.rgb;
                o.pos = UnityObjectToClipPos(v.vertex );
                UNITY_TRANSFER_FOG(o,o.pos);
                TRANSFER_VERTEX_TO_FRAGMENT(o)
                return o;
            }
            float4 frag(VertexOutput i) : COLOR {
                float4 objPos = mul ( unity_ObjectToWorld, float4(0,0,0,1) );
                i.normalDir = normalize(i.normalDir);
                float3x3 tangentTransform = float3x3( i.tangentDir, i.bitangentDir, i.normalDir);
                float3 viewDirection = normalize(_WorldSpaceCameraPos.xyz - i.posWorld.xyz);
                float node_2220_ang = ((_Angle1*3.141592654)/180.0);
                float node_2220_spd = 1.0;
                float node_2220_cos = cos(node_2220_spd*node_2220_ang);
                float node_2220_sin = sin(node_2220_spd*node_2220_ang);
                float2 node_2220_piv = float2(0.5,0.5);
                float2 node_2220 = (mul(i.uv0-node_2220_piv,float2x2( node_2220_cos, -node_2220_sin, node_2220_sin, node_2220_cos))+node_2220_piv);
                float4 node_4748 = _Time + _TimeEditor;
                float2 node_1056 = (i.uv0/4.0);
                float2 node_6038 = (node_2220+(node_1056+(node_4748.g*_AnimationSpeed1)*float2(0,0.6)));
                float3 _NormalWave1Vertical = UnpackNormal(tex2D(_BumpMap,TRANSFORM_TEX(node_6038, _BumpMap)));
                float node_8828_ang = ((_Angle2*3.141592654)/180.0);
                float node_8828_spd = 1.0;
                float node_8828_cos = cos(node_8828_spd*node_8828_ang);
                float node_8828_sin = sin(node_8828_spd*node_8828_ang);
                float2 node_8828_piv = float2(0.5,0.5);
                float2 node_8828 = (mul(i.uv0-node_8828_piv,float2x2( node_8828_cos, -node_8828_sin, node_8828_sin, node_8828_cos))+node_8828_piv);
                float4 node_7920 = _Time + _TimeEditor;
                float2 node_5862 = (node_8828+(1.0 - (node_1056+(node_7920.g*_AnimationSpeed2)*float2(0,0.6))));
                float3 _NormalWave2Horizontal = UnpackNormal(tex2D(_BumpMap,TRANSFORM_TEX(node_5862, _BumpMap)));
                float3 node_5996_nrm_base = lerp(float3(0,0,1),_NormalWave1Vertical.rgb,_NormalIntensity1) + float3(0,0,1);
                float3 node_5996_nrm_detail = lerp(float3(0,0,1),_NormalWave2Horizontal.rgb,_NormalIntensity2) * float3(-1,-1,1);
                float3 node_5996_nrm_combined = node_5996_nrm_base*dot(node_5996_nrm_base, node_5996_nrm_detail)/node_5996_nrm_base.z - node_5996_nrm_detail;
                float3 node_5996 = node_5996_nrm_combined;
                float3 Normalmap = node_5996;
                float3 normalLocal = Normalmap;
                float3 normalDirection = normalize(mul( normalLocal, tangentTransform )); // Perturbed normals
                float3 NormalDirectionMask = mul( unity_WorldToObject, float4(saturate(((abs(i.normalDir)*2.0)*0.5)),0) ).xyz.rgb;
                float3 node_7209 = NormalDirectionMask;
                float3 node_9426 = mul( unity_WorldToObject, float4((i.posWorld.rgb-objPos.rgb),0) ).xyz;
                float2 GB = node_9426.rgb.gb;
                float2 node_7268 = GB;
                float4 _node_1257 = tex2Dlod(_TexAssetMAsk,float4(TRANSFORM_TEX(node_7268, _TexAssetMAsk),0.0,_SmoothMask)); // X Axis FrontBack
                float2 RB = node_9426.rgb.rb;
                float2 node_9779 = RB;
                float4 _node_4330 = tex2Dlod(_TexAssetMAsk,float4(TRANSFORM_TEX(node_9779, _TexAssetMAsk),0.0,_SmoothMask)); // Y Axis TopBottom
                float2 RG = node_9426.rgb.rg;
                float2 node_7670 = RG;
                float4 _node_3650 = tex2Dlod(_TexAssetMAsk,float4(TRANSFORM_TEX(node_7670, _TexAssetMAsk),0.0,_SmoothMask)); // Z Axis LeftRight 
                float node_4586 = (node_7209.r*(_node_1257.r*NormalDirectionMask.r) + node_7209.g*(_node_4330.r*NormalDirectionMask.g) + node_7209.b*(_node_3650.r*NormalDirectionMask.b));
                float node_9251 = 0.0;
                float node_7590 = (1.0+(-1*_Contrast));
                float node_1322 = saturate(((_MaskAmount*2.0+-1.0)+(node_7590 + ( (lerp( node_4586, (1.0 - node_4586), _InvertMask ) - node_9251) * (_Contrast - node_7590) ) / (1.0 - node_9251))));
                float MaskDisslolveCloud = node_1322;
                float CutoutDissolveCloud = MaskDisslolveCloud.r;
                float3 node_1607 = NormalDirectionMask;
                float node_5233 = 0.1;
                float2 node_2619 = (node_5233*GB);
                float4 _node_12570 = tex2Dlod(_WaveMaskDistortionMask,float4(TRANSFORM_TEX(node_2619, _WaveMaskDistortionMask),0.0,_SmoothEdge)); // X Axis FrontBack
                float2 node_8090 = (node_5233*RB);
                float4 _node_43300 = tex2Dlod(_WaveMaskDistortionMask,float4(TRANSFORM_TEX(node_8090, _WaveMaskDistortionMask),0.0,_SmoothEdge)); // Y Axis TopBottom
                float2 node_7721 = (node_5233*RG);
                float4 _node_36500 = tex2Dlod(_WaveMaskDistortionMask,float4(TRANSFORM_TEX(node_7721, _WaveMaskDistortionMask),0.0,_SmoothEdge)); // Z Axis LeftRight 
                float WaveLineMask = (node_1607.r*(_node_12570.r*NormalDirectionMask.r) + node_1607.g*(_node_43300.r*NormalDirectionMask.g) + node_1607.b*(_node_36500.r*NormalDirectionMask.b));
                float4 node_2749 = _Time + _TimeEditor;
                float3 node_9783 = mul( unity_WorldToObject, float4((i.posWorld.rgb-objPos.rgb),0) ).xyz;
                float _SwitchYtoZaxis_var = lerp( node_9783.rgb.g, node_9783.rgb.b, _SwitchYtoZaxis );
                float node_1447 = saturate(((_LinearDissolve+(sin(((_WaveAmout*WaveLineMask)+(node_2749.g*_WaveSpeed)))+((-1*_WaveAmplitude12)*(lerp( _SwitchYtoZaxis_var, (1.0 - _SwitchYtoZaxis_var), _InvertAxis )*2.0+-1.0))))*lerp(2,0,_LineSpread)));
                float Cutout = saturate((CutoutDissolveCloud*node_1447));
                clip(Cutout - 0.5);
                float3 lightDirection = normalize(lerp(_WorldSpaceLightPos0.xyz, _WorldSpaceLightPos0.xyz - i.posWorld.xyz,_WorldSpaceLightPos0.w));
                float3 lightColor = _LightColor0.rgb;
                float3 halfDirection = normalize(viewDirection+lightDirection);
////// Lighting:
                float attenuation = LIGHT_ATTENUATION(i);
                float3 attenColor = attenuation * _LightColor0.xyz;
                float Pi = 3.141592654;
                float InvPi = 0.31830988618;
///////// Gloss:
                float Glossiness = _Glossiness;
                float gloss = Glossiness;
                float perceptualRoughness = 1.0 - Glossiness;
                float roughness = perceptualRoughness * perceptualRoughness;
                float specPow = exp2( gloss * 10.0 + 1.0 );
////// Specular:
                float NdotL = saturate(dot( normalDirection, lightDirection ));
                float LdotH = saturate(dot(lightDirection, halfDirection));
                float3 Specular = (_SpecColor.rgb*_SpecularIntensity);
                float3 specularColor = Specular;
                float specularMonochrome;
                float3 BaseColor = _Color.rgb;
                float3 diffuseColor = BaseColor; // Need this for specular when using metallic
                diffuseColor = EnergyConservationBetweenDiffuseAndSpecular(diffuseColor, specularColor, specularMonochrome);
                specularMonochrome = 1.0-specularMonochrome;
                float NdotV = abs(dot( normalDirection, viewDirection ));
                float NdotH = saturate(dot( normalDirection, halfDirection ));
                float VdotH = saturate(dot( viewDirection, halfDirection ));
                float visTerm = SmithJointGGXVisibilityTerm( NdotL, NdotV, roughness );
                float normTerm = GGXTerm(NdotH, roughness);
                float specularPBL = (visTerm*normTerm) * UNITY_PI;
                #ifdef UNITY_COLORSPACE_GAMMA
                    specularPBL = sqrt(max(1e-4h, specularPBL));
                #endif
                specularPBL = max(0, specularPBL * NdotL);
                #if defined(_SPECULARHIGHLIGHTS_OFF)
                    specularPBL = 0.0;
                #endif
                specularPBL *= any(specularColor) ? 1.0 : 0.0;
                float3 directSpecular = attenColor*specularPBL*FresnelTerm(specularColor, LdotH);
                float3 specular = directSpecular;
/////// Diffuse:
                NdotL = max(0.0,dot( normalDirection, lightDirection ));
                half fd90 = 0.5 + 2 * LdotH * LdotH * (1-gloss);
                float nlPow5 = Pow5(1-NdotL);
                float nvPow5 = Pow5(1-NdotV);
                float3 directDiffuse = ((1 +(fd90 - 1)*nlPow5) * (1 + (fd90 - 1)*nvPow5) * NdotL) * attenColor;
                diffuseColor *= 1-specularMonochrome;
                float3 diffuse = directDiffuse * diffuseColor;
/// Final Color:
                float3 finalColor = diffuse + specular;
                fixed4 finalRGBA = fixed4(finalColor * 1,0);
                UNITY_APPLY_FOG(i.fogCoord, finalRGBA);
                return finalRGBA;
            }
            ENDCG
        }
        Pass {
            Name "ShadowCaster"
            Tags {
                "LightMode"="ShadowCaster"
            }
            Offset 1, 1
            Cull Back
            
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #define UNITY_PASS_SHADOWCASTER
            #define SHOULD_SAMPLE_SH ( defined (LIGHTMAP_OFF) && defined(DYNAMICLIGHTMAP_OFF) )
            #define _GLOSSYENV 1
            #include "UnityCG.cginc"
            #include "Lighting.cginc"
            #include "UnityPBSLighting.cginc"
            #include "UnityStandardBRDF.cginc"
            #pragma fragmentoption ARB_precision_hint_fastest
            #pragma multi_compile_shadowcaster
            #pragma multi_compile LIGHTMAP_OFF LIGHTMAP_ON
            #pragma multi_compile DIRLIGHTMAP_OFF DIRLIGHTMAP_COMBINED DIRLIGHTMAP_SEPARATE
            #pragma multi_compile DYNAMICLIGHTMAP_OFF DYNAMICLIGHTMAP_ON
            #pragma multi_compile_fog
            #pragma only_renderers d3d9 d3d11 glcore gles gles3 metal d3d11_9x xboxone ps4 psp2 n3ds wiiu 
            #pragma target 3.0
            uniform float4 _TimeEditor;
            uniform fixed _SwitchYtoZaxis;
            uniform fixed _InvertAxis;
            uniform float _LinearDissolve;
            uniform float _WaveAmout;
            uniform float _WaveAmplitude12;
            uniform float _WaveSpeed;
            uniform float _Contrast;
            uniform float _MaskAmount;
            uniform fixed _InvertMask;
            uniform sampler2D _TexAssetMAsk; uniform float4 _TexAssetMAsk_ST;
            uniform float _LineSpread;
            uniform float _SmoothEdge;
            uniform sampler2D _WaveMaskDistortionMask; uniform float4 _WaveMaskDistortionMask_ST;
            uniform float _SmoothMask;
            struct VertexInput {
                float4 vertex : POSITION;
                float3 normal : NORMAL;
                float2 texcoord1 : TEXCOORD1;
                float2 texcoord2 : TEXCOORD2;
            };
            struct VertexOutput {
                V2F_SHADOW_CASTER;
                float2 uv1 : TEXCOORD1;
                float2 uv2 : TEXCOORD2;
                float4 posWorld : TEXCOORD3;
                float3 normalDir : TEXCOORD4;
            };
            VertexOutput vert (VertexInput v) {
                VertexOutput o = (VertexOutput)0;
                o.uv1 = v.texcoord1;
                o.uv2 = v.texcoord2;
                o.normalDir = UnityObjectToWorldNormal(v.normal);
                float4 objPos = mul ( unity_ObjectToWorld, float4(0,0,0,1) );
                o.posWorld = mul(unity_ObjectToWorld, v.vertex);
                o.pos = UnityObjectToClipPos(v.vertex );
                TRANSFER_SHADOW_CASTER(o)
                return o;
            }
            float4 frag(VertexOutput i) : COLOR {
                float4 objPos = mul ( unity_ObjectToWorld, float4(0,0,0,1) );
                i.normalDir = normalize(i.normalDir);
                float3 viewDirection = normalize(_WorldSpaceCameraPos.xyz - i.posWorld.xyz);
                float3 normalDirection = i.normalDir;
                float3 NormalDirectionMask = mul( unity_WorldToObject, float4(saturate(((abs(i.normalDir)*2.0)*0.5)),0) ).xyz.rgb;
                float3 node_7209 = NormalDirectionMask;
                float3 node_9426 = mul( unity_WorldToObject, float4((i.posWorld.rgb-objPos.rgb),0) ).xyz;
                float2 GB = node_9426.rgb.gb;
                float2 node_7268 = GB;
                float4 _node_1257 = tex2Dlod(_TexAssetMAsk,float4(TRANSFORM_TEX(node_7268, _TexAssetMAsk),0.0,_SmoothMask)); // X Axis FrontBack
                float2 RB = node_9426.rgb.rb;
                float2 node_9779 = RB;
                float4 _node_4330 = tex2Dlod(_TexAssetMAsk,float4(TRANSFORM_TEX(node_9779, _TexAssetMAsk),0.0,_SmoothMask)); // Y Axis TopBottom
                float2 RG = node_9426.rgb.rg;
                float2 node_7670 = RG;
                float4 _node_3650 = tex2Dlod(_TexAssetMAsk,float4(TRANSFORM_TEX(node_7670, _TexAssetMAsk),0.0,_SmoothMask)); // Z Axis LeftRight 
                float node_4586 = (node_7209.r*(_node_1257.r*NormalDirectionMask.r) + node_7209.g*(_node_4330.r*NormalDirectionMask.g) + node_7209.b*(_node_3650.r*NormalDirectionMask.b));
                float node_9251 = 0.0;
                float node_7590 = (1.0+(-1*_Contrast));
                float node_1322 = saturate(((_MaskAmount*2.0+-1.0)+(node_7590 + ( (lerp( node_4586, (1.0 - node_4586), _InvertMask ) - node_9251) * (_Contrast - node_7590) ) / (1.0 - node_9251))));
                float MaskDisslolveCloud = node_1322;
                float CutoutDissolveCloud = MaskDisslolveCloud.r;
                float3 node_1607 = NormalDirectionMask;
                float node_5233 = 0.1;
                float2 node_2619 = (node_5233*GB);
                float4 _node_12570 = tex2Dlod(_WaveMaskDistortionMask,float4(TRANSFORM_TEX(node_2619, _WaveMaskDistortionMask),0.0,_SmoothEdge)); // X Axis FrontBack
                float2 node_8090 = (node_5233*RB);
                float4 _node_43300 = tex2Dlod(_WaveMaskDistortionMask,float4(TRANSFORM_TEX(node_8090, _WaveMaskDistortionMask),0.0,_SmoothEdge)); // Y Axis TopBottom
                float2 node_7721 = (node_5233*RG);
                float4 _node_36500 = tex2Dlod(_WaveMaskDistortionMask,float4(TRANSFORM_TEX(node_7721, _WaveMaskDistortionMask),0.0,_SmoothEdge)); // Z Axis LeftRight 
                float WaveLineMask = (node_1607.r*(_node_12570.r*NormalDirectionMask.r) + node_1607.g*(_node_43300.r*NormalDirectionMask.g) + node_1607.b*(_node_36500.r*NormalDirectionMask.b));
                float4 node_2749 = _Time + _TimeEditor;
                float3 node_9783 = mul( unity_WorldToObject, float4((i.posWorld.rgb-objPos.rgb),0) ).xyz;
                float _SwitchYtoZaxis_var = lerp( node_9783.rgb.g, node_9783.rgb.b, _SwitchYtoZaxis );
                float node_1447 = saturate(((_LinearDissolve+(sin(((_WaveAmout*WaveLineMask)+(node_2749.g*_WaveSpeed)))+((-1*_WaveAmplitude12)*(lerp( _SwitchYtoZaxis_var, (1.0 - _SwitchYtoZaxis_var), _InvertAxis )*2.0+-1.0))))*lerp(2,0,_LineSpread)));
                float Cutout = saturate((CutoutDissolveCloud*node_1447));
                clip(Cutout - 0.5);
                SHADOW_CASTER_FRAGMENT(i)
            }
            ENDCG
        }
        Pass {
            Name "Meta"
            Tags {
                "LightMode"="Meta"
            }
            Cull Off
            
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #define UNITY_PASS_META 1
            #define SHOULD_SAMPLE_SH ( defined (LIGHTMAP_OFF) && defined(DYNAMICLIGHTMAP_OFF) )
            #define _GLOSSYENV 1
            #include "UnityCG.cginc"
            #include "Lighting.cginc"
            #include "UnityPBSLighting.cginc"
            #include "UnityStandardBRDF.cginc"
            #include "UnityMetaPass.cginc"
            #pragma fragmentoption ARB_precision_hint_fastest
            #pragma multi_compile_shadowcaster
            #pragma multi_compile LIGHTMAP_OFF LIGHTMAP_ON
            #pragma multi_compile DIRLIGHTMAP_OFF DIRLIGHTMAP_COMBINED DIRLIGHTMAP_SEPARATE
            #pragma multi_compile DYNAMICLIGHTMAP_OFF DYNAMICLIGHTMAP_ON
            #pragma multi_compile_fog
            #pragma only_renderers d3d9 d3d11 glcore gles gles3 metal d3d11_9x xboxone ps4 psp2 n3ds wiiu 
            #pragma target 3.0
            uniform float4 _TimeEditor;
            uniform float4 _Color;
            uniform float _SpecularIntensity;
            uniform float _Glossiness;
            uniform float _AnimationSpeed2;
            uniform float _AnimationSpeed1;
            uniform sampler2D _BumpMap; uniform float4 _BumpMap_ST;
            uniform float4 _WaveColorControledbynormalmap;
            uniform float _FresnelColorStrength;
            uniform float4 _FresnelColor;
            uniform float _CausticAnimationSpeed2;
            uniform float _CausticAnimationSpeed1;
            uniform sampler2D _DetailMask; uniform float4 _DetailMask_ST;
            uniform float _Distortion;
            uniform float _CausticSpreadFresnel;
            uniform fixed _node_4314;
            uniform fixed _InvertCausticMask;
            uniform float _WaveColorIntensity;
            uniform float4 _CausticColor;
            uniform float _FresnelContrast;
            uniform fixed _SwitchYtoZaxis;
            uniform fixed _InvertAxis;
            uniform float _LinearDissolve;
            uniform float _WaveAmout;
            uniform float _WaveAmplitude12;
            uniform float _WaveSpeed;
            uniform float _Contrast;
            uniform float _MaskAmount;
            uniform fixed _InvertMask;
            uniform sampler2D _TexAssetMAsk; uniform float4 _TexAssetMAsk_ST;
            uniform float4 _EdgeColor;
            uniform float _DistortionAmount;
            uniform float _EdgeIntensity;
            uniform float _Multiplicator;
            uniform float _Speed;
            uniform float _LineSpread;
            uniform float _SmoothEdge;
            uniform sampler2D _WaveMaskDistortionMask; uniform float4 _WaveMaskDistortionMask_ST;
            uniform float _DistortionSpeed01;
            uniform float _SmoothMask;
            uniform float _CausticAngle2;
            uniform float _causticAngle1;
            uniform float _Angle1;
            uniform float _Angle2;
            struct VertexInput {
                float4 vertex : POSITION;
                float3 normal : NORMAL;
                float2 texcoord0 : TEXCOORD0;
                float2 texcoord1 : TEXCOORD1;
                float2 texcoord2 : TEXCOORD2;
            };
            struct VertexOutput {
                float4 pos : SV_POSITION;
                float2 uv0 : TEXCOORD0;
                float2 uv1 : TEXCOORD1;
                float2 uv2 : TEXCOORD2;
                float4 posWorld : TEXCOORD3;
                float3 normalDir : TEXCOORD4;
            };
            VertexOutput vert (VertexInput v) {
                VertexOutput o = (VertexOutput)0;
                o.uv0 = v.texcoord0;
                o.uv1 = v.texcoord1;
                o.uv2 = v.texcoord2;
                o.normalDir = UnityObjectToWorldNormal(v.normal);
                float4 objPos = mul ( unity_ObjectToWorld, float4(0,0,0,1) );
                o.posWorld = mul(unity_ObjectToWorld, v.vertex);
                o.pos = UnityMetaVertexPosition(v.vertex, v.texcoord1.xy, v.texcoord2.xy, unity_LightmapST, unity_DynamicLightmapST );
                return o;
            }
            float4 frag(VertexOutput i) : SV_Target {
                float4 objPos = mul ( unity_ObjectToWorld, float4(0,0,0,1) );
                i.normalDir = normalize(i.normalDir);
                float3 viewDirection = normalize(_WorldSpaceCameraPos.xyz - i.posWorld.xyz);
                float3 normalDirection = i.normalDir;
                UnityMetaInput o;
                UNITY_INITIALIZE_OUTPUT( UnityMetaInput, o );
                
                float4 node_4409 = _Time + _TimeEditor;
                float2 node_2490 = (i.uv0+(node_4409.g*_Speed)*float2(0,0.1));
                float4 _Burnefect = tex2D(_WaveMaskDistortionMask,TRANSFORM_TEX(node_2490, _WaveMaskDistortionMask));
                float2 node_9496 = lerp(i.uv0,float2(_Burnefect.r,_Burnefect.r),(lerp(0,0.1,_DistortionAmount)*_Multiplicator));
                float4 _node_5086 = tex2D(_WaveMaskDistortionMask,TRANSFORM_TEX(node_9496, _WaveMaskDistortionMask));
                float3 NormalDirectionMask = mul( unity_WorldToObject, float4(saturate(((abs(i.normalDir)*2.0)*0.5)),0) ).xyz.rgb;
                float3 node_1607 = NormalDirectionMask;
                float node_5233 = 0.1;
                float3 node_9426 = mul( unity_WorldToObject, float4((i.posWorld.rgb-objPos.rgb),0) ).xyz;
                float2 GB = node_9426.rgb.gb;
                float2 node_2619 = (node_5233*GB);
                float4 _node_12570 = tex2Dlod(_WaveMaskDistortionMask,float4(TRANSFORM_TEX(node_2619, _WaveMaskDistortionMask),0.0,_SmoothEdge)); // X Axis FrontBack
                float2 RB = node_9426.rgb.rb;
                float2 node_8090 = (node_5233*RB);
                float4 _node_43300 = tex2Dlod(_WaveMaskDistortionMask,float4(TRANSFORM_TEX(node_8090, _WaveMaskDistortionMask),0.0,_SmoothEdge)); // Y Axis TopBottom
                float2 RG = node_9426.rgb.rg;
                float2 node_7721 = (node_5233*RG);
                float4 _node_36500 = tex2Dlod(_WaveMaskDistortionMask,float4(TRANSFORM_TEX(node_7721, _WaveMaskDistortionMask),0.0,_SmoothEdge)); // Z Axis LeftRight 
                float WaveLineMask = (node_1607.r*(_node_12570.r*NormalDirectionMask.r) + node_1607.g*(_node_43300.r*NormalDirectionMask.g) + node_1607.b*(_node_36500.r*NormalDirectionMask.b));
                float4 node_2749 = _Time + _TimeEditor;
                float3 node_9783 = mul( unity_WorldToObject, float4((i.posWorld.rgb-objPos.rgb),0) ).xyz;
                float _SwitchYtoZaxis_var = lerp( node_9783.rgb.g, node_9783.rgb.b, _SwitchYtoZaxis );
                float node_1447 = saturate(((_LinearDissolve+(sin(((_WaveAmout*WaveLineMask)+(node_2749.g*_WaveSpeed)))+((-1*_WaveAmplitude12)*(lerp( _SwitchYtoZaxis_var, (1.0 - _SwitchYtoZaxis_var), _InvertAxis )*2.0+-1.0))))*lerp(2,0,_LineSpread)));
                float EmissiveColorLine = saturate(saturate((node_1447*(1.0 - node_1447))));
                float node_1330 = EmissiveColorLine;
                float3 node_7209 = NormalDirectionMask;
                float2 node_7268 = GB;
                float4 _node_1257 = tex2Dlod(_TexAssetMAsk,float4(TRANSFORM_TEX(node_7268, _TexAssetMAsk),0.0,_SmoothMask)); // X Axis FrontBack
                float2 node_9779 = RB;
                float4 _node_4330 = tex2Dlod(_TexAssetMAsk,float4(TRANSFORM_TEX(node_9779, _TexAssetMAsk),0.0,_SmoothMask)); // Y Axis TopBottom
                float2 node_7670 = RG;
                float4 _node_3650 = tex2Dlod(_TexAssetMAsk,float4(TRANSFORM_TEX(node_7670, _TexAssetMAsk),0.0,_SmoothMask)); // Z Axis LeftRight 
                float node_4586 = (node_7209.r*(_node_1257.r*NormalDirectionMask.r) + node_7209.g*(_node_4330.r*NormalDirectionMask.g) + node_7209.b*(_node_3650.r*NormalDirectionMask.b));
                float node_9251 = 0.0;
                float node_7590 = (1.0+(-1*_Contrast));
                float node_1322 = saturate(((_MaskAmount*2.0+-1.0)+(node_7590 + ( (lerp( node_4586, (1.0 - node_4586), _InvertMask ) - node_9251) * (_Contrast - node_7590) ) / (1.0 - node_9251))));
                float node_8041 = lerp(0.6,1,_EdgeIntensity);
                float node_2708 = 0.0;
                float node_3706 = lerp(-10,0,node_8041); // Rang min
                float3 EmissiveColor = saturate((saturate((saturate(( (node_1330+(1.0 - node_1322)) > 0.5 ? (1.0-(1.0-2.0*((node_1330+(1.0 - node_1322))-0.5))*(1.0-_EdgeColor.rgb)) : (2.0*(node_1330+(1.0 - node_1322))*_EdgeColor.rgb) ))/(1.0-_node_5086.rgb)))*(node_3706 + ( (pow((1.0 - node_1330),lerp(2,0.1,node_8041)) - node_2708) * (lerp(-7,10,node_8041) - node_3706) ) / (1.0 - node_2708))));
                float3 node_6236 = EmissiveColor;
                float node_2220_ang = ((_Angle1*3.141592654)/180.0);
                float node_2220_spd = 1.0;
                float node_2220_cos = cos(node_2220_spd*node_2220_ang);
                float node_2220_sin = sin(node_2220_spd*node_2220_ang);
                float2 node_2220_piv = float2(0.5,0.5);
                float2 node_2220 = (mul(i.uv0-node_2220_piv,float2x2( node_2220_cos, -node_2220_sin, node_2220_sin, node_2220_cos))+node_2220_piv);
                float4 node_4748 = _Time + _TimeEditor;
                float2 node_1056 = (i.uv0/4.0);
                float2 node_6038 = (node_2220+(node_1056+(node_4748.g*_AnimationSpeed1)*float2(0,0.6)));
                float3 _NormalWave1Vertical = UnpackNormal(tex2D(_BumpMap,TRANSFORM_TEX(node_6038, _BumpMap)));
                float node_8828_ang = ((_Angle2*3.141592654)/180.0);
                float node_8828_spd = 1.0;
                float node_8828_cos = cos(node_8828_spd*node_8828_ang);
                float node_8828_sin = sin(node_8828_spd*node_8828_ang);
                float2 node_8828_piv = float2(0.5,0.5);
                float2 node_8828 = (mul(i.uv0-node_8828_piv,float2x2( node_8828_cos, -node_8828_sin, node_8828_sin, node_8828_cos))+node_8828_piv);
                float4 node_7920 = _Time + _TimeEditor;
                float2 node_5862 = (node_8828+(1.0 - (node_1056+(node_7920.g*_AnimationSpeed2)*float2(0,0.6))));
                float3 _NormalWave2Horizontal = UnpackNormal(tex2D(_BumpMap,TRANSFORM_TEX(node_5862, _BumpMap)));
                float WaveMaskColor = (saturate(( _NormalWave2Horizontal.r > 0.5 ? (1.0-(1.0-2.0*(_NormalWave2Horizontal.r-0.5))*(1.0-_NormalWave1Vertical.r)) : (2.0*_NormalWave2Horizontal.r*_NormalWave1Vertical.r) ))*_WaveColorIntensity);
                float4 node_6336 = _Time + _TimeEditor;
                float node_4302 = (node_6336.g*_DistortionSpeed01);
                float2 node_2872 = (i.uv0+node_4302*float2(0,0.2));
                float3 _node_2739 = UnpackNormal(tex2D(_BumpMap,TRANSFORM_TEX(node_2872, _BumpMap)));
                float Distortion = lerp(0,0.2,_Distortion);
                float node_4869_ang = ((_causticAngle1*3.141592654)/180.0);
                float node_4869_spd = 1.0;
                float node_4869_cos = cos(node_4869_spd*node_4869_ang);
                float node_4869_sin = sin(node_4869_spd*node_4869_ang);
                float2 node_4869_piv = float2(0.5,0.5);
                float2 node_4869 = (mul(i.uv0-node_4869_piv,float2x2( node_4869_cos, -node_4869_sin, node_4869_sin, node_4869_cos))+node_4869_piv);
                float4 node_8916 = _Time + _TimeEditor;
                float2 node_1526 = (i.uv0/4.0);
                float2 node_8183 = (lerp(i.uv0,float2(_node_2739.r,_node_2739.r),Distortion)+(node_4869+(node_1526+(node_8916.g*_CausticAnimationSpeed1)*float2(0,0.6))));
                float4 _Caustic1 = tex2D(_DetailMask,TRANSFORM_TEX(node_8183, _DetailMask));
                float node_5854_ang = ((_CausticAngle2*3.141592654)/180.0);
                float node_5854_spd = 1.0;
                float node_5854_cos = cos(node_5854_spd*node_5854_ang);
                float node_5854_sin = sin(node_5854_spd*node_5854_ang);
                float2 node_5854_piv = float2(0.5,0.5);
                float2 node_5854 = (mul(i.uv0-node_5854_piv,float2x2( node_5854_cos, -node_5854_sin, node_5854_sin, node_5854_cos))+node_5854_piv);
                float4 node_1805 = _Time + _TimeEditor;
                float2 node_6610 = (i.uv0+node_4302*float2(0,0.2));
                float3 _node_5242 = UnpackNormal(tex2D(_BumpMap,TRANSFORM_TEX(node_6610, _BumpMap)));
                float2 node_3743 = ((node_5854+(1.0 - (node_1526+(node_1805.g*_CausticAnimationSpeed2)*float2(0,0.6))))+lerp(i.uv0,float2(_node_5242.r,_node_5242.r),Distortion));
                float4 _Caustic2 = tex2D(_DetailMask,TRANSFORM_TEX(node_3743, _DetailMask));
                float2 node_4065 = (i.uv0*2.0+-1.0);
                float3 node_1911 = (saturate((_Caustic1.rgb*_Caustic2.rgb))*(1.0 - ((node_4065*node_4065).r*2.0+-1.0)));
                float3 node_2868 = (node_1911+node_1911);
                float node_4752 = pow(1.0-max(0,dot(normalDirection, viewDirection)),_CausticSpreadFresnel);
                float3 Caustic = (lerp( node_2868, (node_2868*-1.0+1.0), _InvertCausticMask )*lerp( node_4752, (1.0 - node_4752), _node_4314 ));
                float3 node_3814 = (((_WaveColorControledbynormalmap.rgb*WaveMaskColor)+(_CausticColor.rgb*Caustic))+saturate((((0.95*pow(1.0-max(0,dot(normalDirection, viewDirection)),_FresnelContrast))+0.05)*_FresnelColorStrength*_FresnelColor.rgb)));
                float3 EmissiveColorAndCausticFresnel = saturate((node_6236+node_3814));
                o.Emission = EmissiveColorAndCausticFresnel;
                
                float3 BaseColor = _Color.rgb;
                float3 diffColor = BaseColor;
                float3 Specular = (_SpecColor.rgb*_SpecularIntensity);
                float3 specColor = Specular;
                float specularMonochrome = max(max(specColor.r, specColor.g),specColor.b);
                diffColor *= (1.0-specularMonochrome);
                float Glossiness = _Glossiness;
                float roughness = 1.0 - Glossiness;
                o.Albedo = diffColor + specColor * roughness * roughness * 0.5;
                
                return UnityMetaFragment( o );
            }
            ENDCG
        }
    }
    FallBack "Diffuse"
}
