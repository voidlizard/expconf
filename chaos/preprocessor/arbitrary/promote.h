# /* ********************************************************************
#  *                                                                    *
#  *    (C) Copyright Paul Mensonides 2003-2005.                        *
#  *                                                                    *
#  *    Distributed under the Boost Software License, Version 1.0.      *
#  *    (See accompanying file LICENSE).                                *
#  *                                                                    *
#  *    See http://chaos-pp.sourceforge.net for most recent version.    *
#  *                                                                    *
#  ******************************************************************** */
#
# ifndef CHAOS_PREPROCESSOR_ARBITRARY_PROMOTE_H
# define CHAOS_PREPROCESSOR_ARBITRARY_PROMOTE_H
#
# include <chaos/preprocessor/arbitrary/detail/fix.h>
# include <chaos/preprocessor/cat.h>
# include <chaos/preprocessor/config.h>
# include <chaos/preprocessor/control/iif.h>
# include <chaos/preprocessor/detection/is_unary.h>
# include <chaos/preprocessor/lambda/ops.h>
# include <chaos/preprocessor/tuple/eat.h>
#
# /* CHAOS_PP_ARBITRARY_PROMOTE */
#
# define CHAOS_PP_ARBITRARY_PROMOTE(x) \
    CHAOS_PP_IIF(CHAOS_PP_IS_UNARY(x))( \
        CHAOS_PP_ARBITRARY_FIX((CHAOS_PP_CAT(CHAOS_IP_ARBITRARY_PROMOTE_, CHAOS_PP_EAT x))), \
        CHAOS_PP_PRIMITIVE_CAT(CHAOS_IP_ARBITRARY_PROMOTE_, x) \
    ) \
    /**/
#
# define CHAOS_IP_ARBITRARY_PROMOTE_0 (0)
# define CHAOS_IP_ARBITRARY_PROMOTE_1 (1)
# define CHAOS_IP_ARBITRARY_PROMOTE_2 (2)
# define CHAOS_IP_ARBITRARY_PROMOTE_3 (3)
# define CHAOS_IP_ARBITRARY_PROMOTE_4 (4)
# define CHAOS_IP_ARBITRARY_PROMOTE_5 (5)
# define CHAOS_IP_ARBITRARY_PROMOTE_6 (6)
# define CHAOS_IP_ARBITRARY_PROMOTE_7 (7)
# define CHAOS_IP_ARBITRARY_PROMOTE_8 (8)
# define CHAOS_IP_ARBITRARY_PROMOTE_9 (9)
# define CHAOS_IP_ARBITRARY_PROMOTE_10 (1)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_11 (1)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_12 (1)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_13 (1)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_14 (1)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_15 (1)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_16 (1)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_17 (1)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_18 (1)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_19 (1)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_20 (2)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_21 (2)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_22 (2)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_23 (2)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_24 (2)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_25 (2)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_26 (2)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_27 (2)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_28 (2)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_29 (2)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_30 (3)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_31 (3)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_32 (3)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_33 (3)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_34 (3)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_35 (3)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_36 (3)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_37 (3)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_38 (3)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_39 (3)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_40 (4)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_41 (4)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_42 (4)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_43 (4)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_44 (4)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_45 (4)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_46 (4)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_47 (4)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_48 (4)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_49 (4)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_50 (5)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_51 (5)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_52 (5)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_53 (5)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_54 (5)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_55 (5)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_56 (5)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_57 (5)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_58 (5)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_59 (5)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_60 (6)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_61 (6)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_62 (6)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_63 (6)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_64 (6)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_65 (6)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_66 (6)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_67 (6)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_68 (6)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_69 (6)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_70 (7)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_71 (7)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_72 (7)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_73 (7)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_74 (7)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_75 (7)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_76 (7)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_77 (7)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_78 (7)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_79 (7)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_80 (8)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_81 (8)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_82 (8)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_83 (8)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_84 (8)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_85 (8)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_86 (8)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_87 (8)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_88 (8)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_89 (8)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_90 (9)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_91 (9)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_92 (9)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_93 (9)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_94 (9)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_95 (9)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_96 (9)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_97 (9)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_98 (9)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_99 (9)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_100 (1)(0)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_101 (1)(0)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_102 (1)(0)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_103 (1)(0)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_104 (1)(0)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_105 (1)(0)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_106 (1)(0)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_107 (1)(0)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_108 (1)(0)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_109 (1)(0)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_110 (1)(1)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_111 (1)(1)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_112 (1)(1)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_113 (1)(1)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_114 (1)(1)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_115 (1)(1)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_116 (1)(1)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_117 (1)(1)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_118 (1)(1)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_119 (1)(1)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_120 (1)(2)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_121 (1)(2)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_122 (1)(2)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_123 (1)(2)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_124 (1)(2)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_125 (1)(2)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_126 (1)(2)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_127 (1)(2)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_128 (1)(2)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_129 (1)(2)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_130 (1)(3)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_131 (1)(3)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_132 (1)(3)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_133 (1)(3)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_134 (1)(3)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_135 (1)(3)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_136 (1)(3)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_137 (1)(3)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_138 (1)(3)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_139 (1)(3)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_140 (1)(4)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_141 (1)(4)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_142 (1)(4)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_143 (1)(4)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_144 (1)(4)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_145 (1)(4)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_146 (1)(4)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_147 (1)(4)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_148 (1)(4)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_149 (1)(4)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_150 (1)(5)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_151 (1)(5)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_152 (1)(5)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_153 (1)(5)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_154 (1)(5)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_155 (1)(5)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_156 (1)(5)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_157 (1)(5)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_158 (1)(5)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_159 (1)(5)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_160 (1)(6)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_161 (1)(6)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_162 (1)(6)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_163 (1)(6)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_164 (1)(6)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_165 (1)(6)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_166 (1)(6)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_167 (1)(6)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_168 (1)(6)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_169 (1)(6)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_170 (1)(7)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_171 (1)(7)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_172 (1)(7)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_173 (1)(7)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_174 (1)(7)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_175 (1)(7)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_176 (1)(7)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_177 (1)(7)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_178 (1)(7)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_179 (1)(7)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_180 (1)(8)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_181 (1)(8)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_182 (1)(8)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_183 (1)(8)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_184 (1)(8)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_185 (1)(8)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_186 (1)(8)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_187 (1)(8)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_188 (1)(8)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_189 (1)(8)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_190 (1)(9)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_191 (1)(9)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_192 (1)(9)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_193 (1)(9)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_194 (1)(9)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_195 (1)(9)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_196 (1)(9)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_197 (1)(9)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_198 (1)(9)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_199 (1)(9)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_200 (2)(0)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_201 (2)(0)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_202 (2)(0)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_203 (2)(0)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_204 (2)(0)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_205 (2)(0)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_206 (2)(0)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_207 (2)(0)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_208 (2)(0)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_209 (2)(0)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_210 (2)(1)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_211 (2)(1)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_212 (2)(1)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_213 (2)(1)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_214 (2)(1)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_215 (2)(1)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_216 (2)(1)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_217 (2)(1)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_218 (2)(1)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_219 (2)(1)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_220 (2)(2)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_221 (2)(2)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_222 (2)(2)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_223 (2)(2)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_224 (2)(2)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_225 (2)(2)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_226 (2)(2)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_227 (2)(2)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_228 (2)(2)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_229 (2)(2)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_230 (2)(3)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_231 (2)(3)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_232 (2)(3)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_233 (2)(3)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_234 (2)(3)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_235 (2)(3)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_236 (2)(3)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_237 (2)(3)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_238 (2)(3)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_239 (2)(3)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_240 (2)(4)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_241 (2)(4)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_242 (2)(4)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_243 (2)(4)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_244 (2)(4)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_245 (2)(4)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_246 (2)(4)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_247 (2)(4)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_248 (2)(4)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_249 (2)(4)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_250 (2)(5)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_251 (2)(5)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_252 (2)(5)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_253 (2)(5)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_254 (2)(5)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_255 (2)(5)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_256 (2)(5)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_257 (2)(5)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_258 (2)(5)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_259 (2)(5)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_260 (2)(6)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_261 (2)(6)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_262 (2)(6)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_263 (2)(6)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_264 (2)(6)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_265 (2)(6)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_266 (2)(6)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_267 (2)(6)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_268 (2)(6)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_269 (2)(6)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_270 (2)(7)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_271 (2)(7)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_272 (2)(7)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_273 (2)(7)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_274 (2)(7)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_275 (2)(7)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_276 (2)(7)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_277 (2)(7)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_278 (2)(7)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_279 (2)(7)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_280 (2)(8)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_281 (2)(8)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_282 (2)(8)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_283 (2)(8)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_284 (2)(8)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_285 (2)(8)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_286 (2)(8)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_287 (2)(8)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_288 (2)(8)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_289 (2)(8)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_290 (2)(9)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_291 (2)(9)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_292 (2)(9)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_293 (2)(9)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_294 (2)(9)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_295 (2)(9)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_296 (2)(9)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_297 (2)(9)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_298 (2)(9)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_299 (2)(9)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_300 (3)(0)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_301 (3)(0)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_302 (3)(0)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_303 (3)(0)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_304 (3)(0)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_305 (3)(0)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_306 (3)(0)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_307 (3)(0)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_308 (3)(0)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_309 (3)(0)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_310 (3)(1)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_311 (3)(1)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_312 (3)(1)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_313 (3)(1)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_314 (3)(1)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_315 (3)(1)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_316 (3)(1)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_317 (3)(1)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_318 (3)(1)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_319 (3)(1)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_320 (3)(2)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_321 (3)(2)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_322 (3)(2)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_323 (3)(2)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_324 (3)(2)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_325 (3)(2)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_326 (3)(2)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_327 (3)(2)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_328 (3)(2)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_329 (3)(2)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_330 (3)(3)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_331 (3)(3)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_332 (3)(3)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_333 (3)(3)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_334 (3)(3)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_335 (3)(3)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_336 (3)(3)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_337 (3)(3)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_338 (3)(3)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_339 (3)(3)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_340 (3)(4)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_341 (3)(4)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_342 (3)(4)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_343 (3)(4)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_344 (3)(4)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_345 (3)(4)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_346 (3)(4)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_347 (3)(4)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_348 (3)(4)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_349 (3)(4)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_350 (3)(5)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_351 (3)(5)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_352 (3)(5)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_353 (3)(5)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_354 (3)(5)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_355 (3)(5)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_356 (3)(5)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_357 (3)(5)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_358 (3)(5)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_359 (3)(5)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_360 (3)(6)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_361 (3)(6)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_362 (3)(6)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_363 (3)(6)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_364 (3)(6)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_365 (3)(6)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_366 (3)(6)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_367 (3)(6)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_368 (3)(6)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_369 (3)(6)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_370 (3)(7)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_371 (3)(7)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_372 (3)(7)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_373 (3)(7)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_374 (3)(7)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_375 (3)(7)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_376 (3)(7)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_377 (3)(7)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_378 (3)(7)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_379 (3)(7)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_380 (3)(8)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_381 (3)(8)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_382 (3)(8)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_383 (3)(8)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_384 (3)(8)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_385 (3)(8)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_386 (3)(8)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_387 (3)(8)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_388 (3)(8)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_389 (3)(8)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_390 (3)(9)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_391 (3)(9)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_392 (3)(9)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_393 (3)(9)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_394 (3)(9)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_395 (3)(9)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_396 (3)(9)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_397 (3)(9)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_398 (3)(9)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_399 (3)(9)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_400 (4)(0)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_401 (4)(0)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_402 (4)(0)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_403 (4)(0)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_404 (4)(0)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_405 (4)(0)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_406 (4)(0)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_407 (4)(0)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_408 (4)(0)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_409 (4)(0)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_410 (4)(1)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_411 (4)(1)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_412 (4)(1)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_413 (4)(1)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_414 (4)(1)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_415 (4)(1)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_416 (4)(1)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_417 (4)(1)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_418 (4)(1)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_419 (4)(1)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_420 (4)(2)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_421 (4)(2)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_422 (4)(2)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_423 (4)(2)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_424 (4)(2)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_425 (4)(2)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_426 (4)(2)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_427 (4)(2)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_428 (4)(2)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_429 (4)(2)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_430 (4)(3)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_431 (4)(3)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_432 (4)(3)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_433 (4)(3)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_434 (4)(3)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_435 (4)(3)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_436 (4)(3)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_437 (4)(3)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_438 (4)(3)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_439 (4)(3)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_440 (4)(4)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_441 (4)(4)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_442 (4)(4)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_443 (4)(4)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_444 (4)(4)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_445 (4)(4)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_446 (4)(4)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_447 (4)(4)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_448 (4)(4)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_449 (4)(4)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_450 (4)(5)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_451 (4)(5)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_452 (4)(5)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_453 (4)(5)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_454 (4)(5)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_455 (4)(5)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_456 (4)(5)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_457 (4)(5)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_458 (4)(5)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_459 (4)(5)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_460 (4)(6)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_461 (4)(6)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_462 (4)(6)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_463 (4)(6)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_464 (4)(6)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_465 (4)(6)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_466 (4)(6)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_467 (4)(6)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_468 (4)(6)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_469 (4)(6)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_470 (4)(7)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_471 (4)(7)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_472 (4)(7)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_473 (4)(7)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_474 (4)(7)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_475 (4)(7)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_476 (4)(7)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_477 (4)(7)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_478 (4)(7)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_479 (4)(7)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_480 (4)(8)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_481 (4)(8)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_482 (4)(8)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_483 (4)(8)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_484 (4)(8)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_485 (4)(8)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_486 (4)(8)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_487 (4)(8)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_488 (4)(8)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_489 (4)(8)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_490 (4)(9)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_491 (4)(9)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_492 (4)(9)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_493 (4)(9)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_494 (4)(9)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_495 (4)(9)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_496 (4)(9)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_497 (4)(9)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_498 (4)(9)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_499 (4)(9)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_500 (5)(0)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_501 (5)(0)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_502 (5)(0)(2)
# define CHAOS_IP_ARBITRARY_PROMOTE_503 (5)(0)(3)
# define CHAOS_IP_ARBITRARY_PROMOTE_504 (5)(0)(4)
# define CHAOS_IP_ARBITRARY_PROMOTE_505 (5)(0)(5)
# define CHAOS_IP_ARBITRARY_PROMOTE_506 (5)(0)(6)
# define CHAOS_IP_ARBITRARY_PROMOTE_507 (5)(0)(7)
# define CHAOS_IP_ARBITRARY_PROMOTE_508 (5)(0)(8)
# define CHAOS_IP_ARBITRARY_PROMOTE_509 (5)(0)(9)
# define CHAOS_IP_ARBITRARY_PROMOTE_510 (5)(1)(0)
# define CHAOS_IP_ARBITRARY_PROMOTE_511 (5)(1)(1)
# define CHAOS_IP_ARBITRARY_PROMOTE_512 (5)(1)(2)
#
# endif
