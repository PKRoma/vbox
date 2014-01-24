/* DO NOT EDIT - THIS FILE IS AUTOMATICALLY GENERATED */
#ifndef _GLX_g_disptab_h_
#define _GLX_g_disptab_h_
/*
 * SGI FREE SOFTWARE LICENSE B (Version 2.0, Sept. 18, 2008)
 * Copyright (C) 1991-2000 Silicon Graphics, Inc. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice including the dates of first publication and
 * either this permission notice or a reference to
 * http://oss.sgi.com/projects/FreeB/
 * shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * SILICON GRAPHICS, INC. BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of Silicon Graphics, Inc.
 * shall not be used in advertising or otherwise to promote the sale, use or
 * other dealings in this Software without prior written authorization from
 * Silicon Graphics, Inc.
 */

extern int __glXRender(__GLXclientState *, GLbyte *);
extern int __glXRenderLarge(__GLXclientState *, GLbyte *);
extern int __glXSendLargeCommand(__GLXclientState * cl,
                                 GLXContextTag contextTag);
extern int __glXCreateContext(__GLXclientState *, GLbyte *);
extern int __glXCreateNewContext(__GLXclientState * cl, GLbyte * pc);
extern int __glXDestroyContext(__GLXclientState *, GLbyte *);
extern int __glXMakeCurrent(__GLXclientState *, GLbyte *);
extern int __glXMakeContextCurrent(__GLXclientState *, GLbyte *);
extern int __glXCreatePbuffer(__GLXclientState * cl, GLbyte * pc);
extern int __glXDestroyPbuffer(__GLXclientState * cl, GLbyte * pc);
extern int __glXGetDrawableAttributes(__GLXclientState * cl, GLbyte * pc);
extern int __glXChangeDrawableAttributes(__GLXclientState * cl, GLbyte * pc);
extern int __glXIsDirect(__GLXclientState *, GLbyte *);
extern int __glXQueryVersion(__GLXclientState *, GLbyte *);
extern int __glXWaitGL(__GLXclientState *, GLbyte *);
extern int __glXWaitX(__GLXclientState *, GLbyte *);
extern int __glXCopyContext(__GLXclientState *, GLbyte *);
extern int __glXSwapBuffers(__GLXclientState *, GLbyte *);
extern int __glXUseXFont(__GLXclientState *, GLbyte *);
extern int __glXCreateGLXPixmap(__GLXclientState *, GLbyte *);
extern int __glXCreatePixmap(__GLXclientState * cl, GLbyte * pc);
extern int __glXGetVisualConfigs(__GLXclientState *, GLbyte *);
extern int __glXDestroyGLXPixmap(__GLXclientState *, GLbyte *);
extern int __glXVendorPrivate(__GLXclientState *, GLbyte *);
extern int __glXVendorPrivateWithReply(__GLXclientState *, GLbyte *);
extern int __glXQueryExtensionsString(__GLXclientState *, GLbyte *);
extern int __glXQueryServerString(__GLXclientState *, GLbyte *);
extern int __glXClientInfo(__GLXclientState *, GLbyte *);
extern int __glXGetFBConfigs(__GLXclientState *, GLbyte *);
extern int __glXCreateWindow(__GLXclientState * cl, GLbyte * pc);
extern int __glXDestroyWindow(__GLXclientState * cl, GLbyte * pc);
extern int __glXQueryContext(__GLXclientState * cl, GLbyte * pc);
extern int __glXDisp_NewList(__GLXclientState *, GLbyte *);
extern int __glXDisp_EndList(__GLXclientState *, GLbyte *);
extern int __glXDisp_DeleteLists(__GLXclientState *, GLbyte *);
extern int __glXDisp_GenLists(__GLXclientState *, GLbyte *);
extern int __glXDisp_FeedbackBuffer(__GLXclientState *, GLbyte *);
extern int __glXDisp_SelectBuffer(__GLXclientState *, GLbyte *);
extern int __glXDisp_RenderMode(__GLXclientState *, GLbyte *);
extern int __glXDisp_Finish(__GLXclientState *, GLbyte *);
extern int __glXDisp_PixelStoref(__GLXclientState *, GLbyte *);
extern int __glXDisp_PixelStorei(__GLXclientState *, GLbyte *);
extern int __glXDisp_ReadPixels(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetBooleanv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetClipPlane(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetDoublev(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetError(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetFloatv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetIntegerv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetLightfv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetLightiv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetMapdv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetMapfv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetMapiv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetMaterialfv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetMaterialiv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetPixelMapfv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetPixelMapuiv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetPixelMapusv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetPolygonStipple(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetString(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetTexEnvfv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetTexEnviv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetTexGendv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetTexGenfv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetTexGeniv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetTexImage(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetTexParameterfv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetTexParameteriv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetTexLevelParameterfv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetTexLevelParameteriv(__GLXclientState *, GLbyte *);
extern int __glXDisp_IsEnabled(__GLXclientState *, GLbyte *);
extern int __glXDisp_IsList(__GLXclientState *, GLbyte *);
extern int __glXDisp_Flush(__GLXclientState *, GLbyte *);
extern int __glXDisp_AreTexturesResident(__GLXclientState *, GLbyte *);
extern int __glXDisp_DeleteTextures(__GLXclientState *, GLbyte *);
extern int __glXDisp_GenTextures(__GLXclientState *, GLbyte *);
extern int __glXDisp_IsTexture(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetColorTable(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetColorTableParameterfv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetColorTableParameteriv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetConvolutionFilter(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetConvolutionParameterfv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetConvolutionParameteriv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetSeparableFilter(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetHistogram(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetHistogramParameterfv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetHistogramParameteriv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetMinmax(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetMinmaxParameterfv(__GLXclientState *, GLbyte *);
extern int __glXDisp_GetMinmaxParameteriv(__GLXclientState *, GLbyte *);

extern void __glXDisp_CallList(GLbyte *);
extern void __glXDisp_CallLists(GLbyte *);
extern void __glXDisp_ListBase(GLbyte *);
extern void __glXDisp_Begin(GLbyte *);
extern void __glXDisp_Bitmap(GLbyte *);
extern void __glXDisp_Color3bv(GLbyte *);
extern void __glXDisp_Color3dv(GLbyte *);
extern void __glXDisp_Color3fv(GLbyte *);
extern void __glXDisp_Color3iv(GLbyte *);
extern void __glXDisp_Color3sv(GLbyte *);
extern void __glXDisp_Color3ubv(GLbyte *);
extern void __glXDisp_Color3uiv(GLbyte *);
extern void __glXDisp_Color3usv(GLbyte *);
extern void __glXDisp_Color4bv(GLbyte *);
extern void __glXDisp_Color4dv(GLbyte *);
extern void __glXDisp_Color4fv(GLbyte *);
extern void __glXDisp_Color4iv(GLbyte *);
extern void __glXDisp_Color4sv(GLbyte *);
extern void __glXDisp_Color4ubv(GLbyte *);
extern void __glXDisp_Color4uiv(GLbyte *);
extern void __glXDisp_Color4usv(GLbyte *);
extern void __glXDisp_EdgeFlagv(GLbyte *);
extern void __glXDisp_End(GLbyte *);
extern void __glXDisp_Indexdv(GLbyte *);
extern void __glXDisp_Indexfv(GLbyte *);
extern void __glXDisp_Indexiv(GLbyte *);
extern void __glXDisp_Indexsv(GLbyte *);
extern void __glXDisp_Normal3bv(GLbyte *);
extern void __glXDisp_Normal3dv(GLbyte *);
extern void __glXDisp_Normal3fv(GLbyte *);
extern void __glXDisp_Normal3iv(GLbyte *);
extern void __glXDisp_Normal3sv(GLbyte *);
extern void __glXDisp_RasterPos2dv(GLbyte *);
extern void __glXDisp_RasterPos2fv(GLbyte *);
extern void __glXDisp_RasterPos2iv(GLbyte *);
extern void __glXDisp_RasterPos2sv(GLbyte *);
extern void __glXDisp_RasterPos3dv(GLbyte *);
extern void __glXDisp_RasterPos3fv(GLbyte *);
extern void __glXDisp_RasterPos3iv(GLbyte *);
extern void __glXDisp_RasterPos3sv(GLbyte *);
extern void __glXDisp_RasterPos4dv(GLbyte *);
extern void __glXDisp_RasterPos4fv(GLbyte *);
extern void __glXDisp_RasterPos4iv(GLbyte *);
extern void __glXDisp_RasterPos4sv(GLbyte *);
extern void __glXDisp_Rectdv(GLbyte *);
extern void __glXDisp_Rectfv(GLbyte *);
extern void __glXDisp_Rectiv(GLbyte *);
extern void __glXDisp_Rectsv(GLbyte *);
extern void __glXDisp_TexCoord1dv(GLbyte *);
extern void __glXDisp_TexCoord1fv(GLbyte *);
extern void __glXDisp_TexCoord1iv(GLbyte *);
extern void __glXDisp_TexCoord1sv(GLbyte *);
extern void __glXDisp_TexCoord2dv(GLbyte *);
extern void __glXDisp_TexCoord2fv(GLbyte *);
extern void __glXDisp_TexCoord2iv(GLbyte *);
extern void __glXDisp_TexCoord2sv(GLbyte *);
extern void __glXDisp_TexCoord3dv(GLbyte *);
extern void __glXDisp_TexCoord3fv(GLbyte *);
extern void __glXDisp_TexCoord3iv(GLbyte *);
extern void __glXDisp_TexCoord3sv(GLbyte *);
extern void __glXDisp_TexCoord4dv(GLbyte *);
extern void __glXDisp_TexCoord4fv(GLbyte *);
extern void __glXDisp_TexCoord4iv(GLbyte *);
extern void __glXDisp_TexCoord4sv(GLbyte *);
extern void __glXDisp_Vertex2dv(GLbyte *);
extern void __glXDisp_Vertex2fv(GLbyte *);
extern void __glXDisp_Vertex2iv(GLbyte *);
extern void __glXDisp_Vertex2sv(GLbyte *);
extern void __glXDisp_Vertex3dv(GLbyte *);
extern void __glXDisp_Vertex3fv(GLbyte *);
extern void __glXDisp_Vertex3iv(GLbyte *);
extern void __glXDisp_Vertex3sv(GLbyte *);
extern void __glXDisp_Vertex4dv(GLbyte *);
extern void __glXDisp_Vertex4fv(GLbyte *);
extern void __glXDisp_Vertex4iv(GLbyte *);
extern void __glXDisp_Vertex4sv(GLbyte *);
extern void __glXDisp_ClipPlane(GLbyte *);
extern void __glXDisp_ColorMaterial(GLbyte *);
extern void __glXDisp_CullFace(GLbyte *);
extern void __glXDisp_Fogf(GLbyte *);
extern void __glXDisp_Fogfv(GLbyte *);
extern void __glXDisp_Fogi(GLbyte *);
extern void __glXDisp_Fogiv(GLbyte *);
extern void __glXDisp_FrontFace(GLbyte *);
extern void __glXDisp_Hint(GLbyte *);
extern void __glXDisp_Lightf(GLbyte *);
extern void __glXDisp_Lightfv(GLbyte *);
extern void __glXDisp_Lighti(GLbyte *);
extern void __glXDisp_Lightiv(GLbyte *);
extern void __glXDisp_LightModelf(GLbyte *);
extern void __glXDisp_LightModelfv(GLbyte *);
extern void __glXDisp_LightModeli(GLbyte *);
extern void __glXDisp_LightModeliv(GLbyte *);
extern void __glXDisp_LineStipple(GLbyte *);
extern void __glXDisp_LineWidth(GLbyte *);
extern void __glXDisp_Materialf(GLbyte *);
extern void __glXDisp_Materialfv(GLbyte *);
extern void __glXDisp_Materiali(GLbyte *);
extern void __glXDisp_Materialiv(GLbyte *);
extern void __glXDisp_PointSize(GLbyte *);
extern void __glXDisp_PolygonMode(GLbyte *);
extern void __glXDisp_PolygonStipple(GLbyte *);
extern void __glXDisp_Scissor(GLbyte *);
extern void __glXDisp_ShadeModel(GLbyte *);
extern void __glXDisp_TexParameterf(GLbyte *);
extern void __glXDisp_TexParameterfv(GLbyte *);
extern void __glXDisp_TexParameteri(GLbyte *);
extern void __glXDisp_TexParameteriv(GLbyte *);
extern void __glXDisp_TexImage1D(GLbyte *);
extern void __glXDisp_TexImage2D(GLbyte *);
extern void __glXDisp_TexEnvf(GLbyte *);
extern void __glXDisp_TexEnvfv(GLbyte *);
extern void __glXDisp_TexEnvi(GLbyte *);
extern void __glXDisp_TexEnviv(GLbyte *);
extern void __glXDisp_TexGend(GLbyte *);
extern void __glXDisp_TexGendv(GLbyte *);
extern void __glXDisp_TexGenf(GLbyte *);
extern void __glXDisp_TexGenfv(GLbyte *);
extern void __glXDisp_TexGeni(GLbyte *);
extern void __glXDisp_TexGeniv(GLbyte *);
extern void __glXDisp_InitNames(GLbyte *);
extern void __glXDisp_LoadName(GLbyte *);
extern void __glXDisp_PassThrough(GLbyte *);
extern void __glXDisp_PopName(GLbyte *);
extern void __glXDisp_PushName(GLbyte *);
extern void __glXDisp_DrawBuffer(GLbyte *);
extern void __glXDisp_Clear(GLbyte *);
extern void __glXDisp_ClearAccum(GLbyte *);
extern void __glXDisp_ClearIndex(GLbyte *);
extern void __glXDisp_ClearColor(GLbyte *);
extern void __glXDisp_ClearStencil(GLbyte *);
extern void __glXDisp_ClearDepth(GLbyte *);
extern void __glXDisp_StencilMask(GLbyte *);
extern void __glXDisp_ColorMask(GLbyte *);
extern void __glXDisp_DepthMask(GLbyte *);
extern void __glXDisp_IndexMask(GLbyte *);
extern void __glXDisp_Accum(GLbyte *);
extern void __glXDisp_Disable(GLbyte *);
extern void __glXDisp_Enable(GLbyte *);
extern void __glXDisp_PopAttrib(GLbyte *);
extern void __glXDisp_PushAttrib(GLbyte *);
extern void __glXDisp_Map1d(GLbyte *);
extern void __glXDisp_Map1f(GLbyte *);
extern void __glXDisp_Map2d(GLbyte *);
extern void __glXDisp_Map2f(GLbyte *);
extern void __glXDisp_MapGrid1d(GLbyte *);
extern void __glXDisp_MapGrid1f(GLbyte *);
extern void __glXDisp_MapGrid2d(GLbyte *);
extern void __glXDisp_MapGrid2f(GLbyte *);
extern void __glXDisp_EvalCoord1dv(GLbyte *);
extern void __glXDisp_EvalCoord1fv(GLbyte *);
extern void __glXDisp_EvalCoord2dv(GLbyte *);
extern void __glXDisp_EvalCoord2fv(GLbyte *);
extern void __glXDisp_EvalMesh1(GLbyte *);
extern void __glXDisp_EvalPoint1(GLbyte *);
extern void __glXDisp_EvalMesh2(GLbyte *);
extern void __glXDisp_EvalPoint2(GLbyte *);
extern void __glXDisp_AlphaFunc(GLbyte *);
extern void __glXDisp_BlendFunc(GLbyte *);
extern void __glXDisp_LogicOp(GLbyte *);
extern void __glXDisp_StencilFunc(GLbyte *);
extern void __glXDisp_StencilOp(GLbyte *);
extern void __glXDisp_DepthFunc(GLbyte *);
extern void __glXDisp_PixelZoom(GLbyte *);
extern void __glXDisp_PixelTransferf(GLbyte *);
extern void __glXDisp_PixelTransferi(GLbyte *);
extern void __glXDisp_PixelMapfv(GLbyte *);
extern void __glXDisp_PixelMapuiv(GLbyte *);
extern void __glXDisp_PixelMapusv(GLbyte *);
extern void __glXDisp_ReadBuffer(GLbyte *);
extern void __glXDisp_CopyPixels(GLbyte *);
extern void __glXDisp_DrawPixels(GLbyte *);
extern void __glXDisp_DepthRange(GLbyte *);
extern void __glXDisp_Frustum(GLbyte *);
extern void __glXDisp_LoadIdentity(GLbyte *);
extern void __glXDisp_LoadMatrixf(GLbyte *);
extern void __glXDisp_LoadMatrixd(GLbyte *);
extern void __glXDisp_MatrixMode(GLbyte *);
extern void __glXDisp_MultMatrixf(GLbyte *);
extern void __glXDisp_MultMatrixd(GLbyte *);
extern void __glXDisp_Ortho(GLbyte *);
extern void __glXDisp_PopMatrix(GLbyte *);
extern void __glXDisp_PushMatrix(GLbyte *);
extern void __glXDisp_Rotated(GLbyte *);
extern void __glXDisp_Rotatef(GLbyte *);
extern void __glXDisp_Scaled(GLbyte *);
extern void __glXDisp_Scalef(GLbyte *);
extern void __glXDisp_Translated(GLbyte *);
extern void __glXDisp_Translatef(GLbyte *);
extern void __glXDisp_Viewport(GLbyte *);
extern void __glXDisp_PolygonOffset(GLbyte *);
extern void __glXDisp_DrawArrays(GLbyte *);
extern void __glXDisp_Indexubv(GLbyte *);
extern void __glXDisp_ColorSubTable(GLbyte *);
extern void __glXDisp_CopyColorSubTable(GLbyte *);
extern void __glXDisp_ActiveTextureARB(GLbyte *);
extern void __glXDisp_MultiTexCoord1dvARB(GLbyte *);
extern void __glXDisp_MultiTexCoord1fvARB(GLbyte *);
extern void __glXDisp_MultiTexCoord1ivARB(GLbyte *);
extern void __glXDisp_MultiTexCoord1svARB(GLbyte *);
extern void __glXDisp_MultiTexCoord2dvARB(GLbyte *);
extern void __glXDisp_MultiTexCoord2fvARB(GLbyte *);
extern void __glXDisp_MultiTexCoord2ivARB(GLbyte *);
extern void __glXDisp_MultiTexCoord2svARB(GLbyte *);
extern void __glXDisp_MultiTexCoord3dvARB(GLbyte *);
extern void __glXDisp_MultiTexCoord3fvARB(GLbyte *);
extern void __glXDisp_MultiTexCoord3ivARB(GLbyte *);
extern void __glXDisp_MultiTexCoord3svARB(GLbyte *);
extern void __glXDisp_MultiTexCoord4dvARB(GLbyte *);
extern void __glXDisp_MultiTexCoord4fvARB(GLbyte *);
extern void __glXDisp_MultiTexCoord4ivARB(GLbyte *);
extern void __glXDisp_MultiTexCoord4svARB(GLbyte *);

extern int __glXSwapRender(__GLXclientState *, GLbyte *);
extern int __glXSwapRenderLarge(__GLXclientState *, GLbyte *);
extern int __glXSwapCreateContext(__GLXclientState *, GLbyte *);
extern int __glXSwapCreateNewContext(__GLXclientState * cl, GLbyte * pc);
extern int __glXSwapDestroyContext(__GLXclientState *, GLbyte *);
extern int __glXSwapMakeCurrent(__GLXclientState *, GLbyte *);
extern int __glXSwapMakeContextCurrent(__GLXclientState *, GLbyte *);
extern int __glXSwapCreatePbuffer(__GLXclientState * cl, GLbyte * pc);
extern int __glXSwapDestroyPbuffer(__GLXclientState * cl, GLbyte * pc);
extern int __glXSwapGetDrawableAttributes(__GLXclientState * cl, GLbyte * pc);
extern int __glXSwapChangeDrawableAttributes(__GLXclientState * cl,
                                             GLbyte * pc);
extern int __glXSwapIsDirect(__GLXclientState *, GLbyte *);
extern int __glXSwapQueryVersion(__GLXclientState *, GLbyte *);
extern int __glXSwapWaitGL(__GLXclientState *, GLbyte *);
extern int __glXSwapWaitX(__GLXclientState *, GLbyte *);
extern int __glXSwapCopyContext(__GLXclientState *, GLbyte *);
extern int __glXSwapSwapBuffers(__GLXclientState *, GLbyte *);
extern int __glXSwapUseXFont(__GLXclientState *, GLbyte *);
extern int __glXSwapCreateGLXPixmap(__GLXclientState *, GLbyte *);
extern int __glXSwapCreatePixmap(__GLXclientState * cl, GLbyte * pc);
extern int __glXSwapGetVisualConfigs(__GLXclientState *, GLbyte *);
extern int __glXSwapDestroyGLXPixmap(__GLXclientState *, GLbyte *);
extern int __glXSwapVendorPrivate(__GLXclientState *, GLbyte *);
extern int __glXSwapVendorPrivateWithReply(__GLXclientState *, GLbyte *);
extern int __glXSwapQueryExtensionsString(__GLXclientState *, GLbyte *);
extern int __glXSwapQueryServerString(__GLXclientState *, GLbyte *);
extern int __glXSwapClientInfo(__GLXclientState *, GLbyte *);
extern int __glXSwapGetFBConfigs(__GLXclientState *, GLbyte *);
extern int __glXSwapCreateWindow(__GLXclientState * cl, GLbyte * pc);
extern int __glXSwapDestroyWindow(__GLXclientState * cl, GLbyte * pc);
extern int __glXSwapQueryContext(__GLXclientState * cl, GLbyte * pc);
extern int __glXDispSwap_NewList(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_EndList(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_DeleteLists(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GenLists(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_FeedbackBuffer(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_SelectBuffer(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_RenderMode(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_Finish(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_PixelStoref(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_PixelStorei(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_ReadPixels(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetBooleanv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetClipPlane(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetDoublev(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetError(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetFloatv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetIntegerv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetLightfv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetLightiv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetMapdv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetMapfv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetMapiv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetMaterialfv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetMaterialiv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetPixelMapfv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetPixelMapuiv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetPixelMapusv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetPolygonStipple(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetString(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetTexEnvfv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetTexEnviv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetTexGendv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetTexGenfv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetTexGeniv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetTexImage(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetTexParameterfv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetTexParameteriv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetTexLevelParameterfv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetTexLevelParameteriv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_IsEnabled(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_IsList(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_Flush(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_AreTexturesResident(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_DeleteTextures(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GenTextures(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_IsTexture(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetColorTable(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetColorTableParameterfv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetColorTableParameteriv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetConvolutionFilter(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetConvolutionParameterfv(__GLXclientState *,
                                                   GLbyte *);
extern int __glXDispSwap_GetConvolutionParameteriv(__GLXclientState *,
                                                   GLbyte *);
extern int __glXDispSwap_GetSeparableFilter(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetHistogram(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetHistogramParameterfv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetHistogramParameteriv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetMinmax(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetMinmaxParameterfv(__GLXclientState *, GLbyte *);
extern int __glXDispSwap_GetMinmaxParameteriv(__GLXclientState *, GLbyte *);

extern void __glXDispSwap_CallList(GLbyte *);
extern void __glXDispSwap_CallLists(GLbyte *);
extern void __glXDispSwap_ListBase(GLbyte *);
extern void __glXDispSwap_Begin(GLbyte *);
extern void __glXDispSwap_Bitmap(GLbyte *);
extern void __glXDispSwap_Color3bv(GLbyte *);
extern void __glXDispSwap_Color3dv(GLbyte *);
extern void __glXDispSwap_Color3fv(GLbyte *);
extern void __glXDispSwap_Color3iv(GLbyte *);
extern void __glXDispSwap_Color3sv(GLbyte *);
extern void __glXDispSwap_Color3ubv(GLbyte *);
extern void __glXDispSwap_Color3uiv(GLbyte *);
extern void __glXDispSwap_Color3usv(GLbyte *);
extern void __glXDispSwap_Color4bv(GLbyte *);
extern void __glXDispSwap_Color4dv(GLbyte *);
extern void __glXDispSwap_Color4fv(GLbyte *);
extern void __glXDispSwap_Color4iv(GLbyte *);
extern void __glXDispSwap_Color4sv(GLbyte *);
extern void __glXDispSwap_Color4ubv(GLbyte *);
extern void __glXDispSwap_Color4uiv(GLbyte *);
extern void __glXDispSwap_Color4usv(GLbyte *);
extern void __glXDispSwap_EdgeFlagv(GLbyte *);
extern void __glXDispSwap_End(GLbyte *);
extern void __glXDispSwap_Indexdv(GLbyte *);
extern void __glXDispSwap_Indexfv(GLbyte *);
extern void __glXDispSwap_Indexiv(GLbyte *);
extern void __glXDispSwap_Indexsv(GLbyte *);
extern void __glXDispSwap_Normal3bv(GLbyte *);
extern void __glXDispSwap_Normal3dv(GLbyte *);
extern void __glXDispSwap_Normal3fv(GLbyte *);
extern void __glXDispSwap_Normal3iv(GLbyte *);
extern void __glXDispSwap_Normal3sv(GLbyte *);
extern void __glXDispSwap_RasterPos2dv(GLbyte *);
extern void __glXDispSwap_RasterPos2fv(GLbyte *);
extern void __glXDispSwap_RasterPos2iv(GLbyte *);
extern void __glXDispSwap_RasterPos2sv(GLbyte *);
extern void __glXDispSwap_RasterPos3dv(GLbyte *);
extern void __glXDispSwap_RasterPos3fv(GLbyte *);
extern void __glXDispSwap_RasterPos3iv(GLbyte *);
extern void __glXDispSwap_RasterPos3sv(GLbyte *);
extern void __glXDispSwap_RasterPos4dv(GLbyte *);
extern void __glXDispSwap_RasterPos4fv(GLbyte *);
extern void __glXDispSwap_RasterPos4iv(GLbyte *);
extern void __glXDispSwap_RasterPos4sv(GLbyte *);
extern void __glXDispSwap_Rectdv(GLbyte *);
extern void __glXDispSwap_Rectfv(GLbyte *);
extern void __glXDispSwap_Rectiv(GLbyte *);
extern void __glXDispSwap_Rectsv(GLbyte *);
extern void __glXDispSwap_TexCoord1dv(GLbyte *);
extern void __glXDispSwap_TexCoord1fv(GLbyte *);
extern void __glXDispSwap_TexCoord1iv(GLbyte *);
extern void __glXDispSwap_TexCoord1sv(GLbyte *);
extern void __glXDispSwap_TexCoord2dv(GLbyte *);
extern void __glXDispSwap_TexCoord2fv(GLbyte *);
extern void __glXDispSwap_TexCoord2iv(GLbyte *);
extern void __glXDispSwap_TexCoord2sv(GLbyte *);
extern void __glXDispSwap_TexCoord3dv(GLbyte *);
extern void __glXDispSwap_TexCoord3fv(GLbyte *);
extern void __glXDispSwap_TexCoord3iv(GLbyte *);
extern void __glXDispSwap_TexCoord3sv(GLbyte *);
extern void __glXDispSwap_TexCoord4dv(GLbyte *);
extern void __glXDispSwap_TexCoord4fv(GLbyte *);
extern void __glXDispSwap_TexCoord4iv(GLbyte *);
extern void __glXDispSwap_TexCoord4sv(GLbyte *);
extern void __glXDispSwap_Vertex2dv(GLbyte *);
extern void __glXDispSwap_Vertex2fv(GLbyte *);
extern void __glXDispSwap_Vertex2iv(GLbyte *);
extern void __glXDispSwap_Vertex2sv(GLbyte *);
extern void __glXDispSwap_Vertex3dv(GLbyte *);
extern void __glXDispSwap_Vertex3fv(GLbyte *);
extern void __glXDispSwap_Vertex3iv(GLbyte *);
extern void __glXDispSwap_Vertex3sv(GLbyte *);
extern void __glXDispSwap_Vertex4dv(GLbyte *);
extern void __glXDispSwap_Vertex4fv(GLbyte *);
extern void __glXDispSwap_Vertex4iv(GLbyte *);
extern void __glXDispSwap_Vertex4sv(GLbyte *);
extern void __glXDispSwap_ClipPlane(GLbyte *);
extern void __glXDispSwap_ColorMaterial(GLbyte *);
extern void __glXDispSwap_CullFace(GLbyte *);
extern void __glXDispSwap_Fogf(GLbyte *);
extern void __glXDispSwap_Fogfv(GLbyte *);
extern void __glXDispSwap_Fogi(GLbyte *);
extern void __glXDispSwap_Fogiv(GLbyte *);
extern void __glXDispSwap_FrontFace(GLbyte *);
extern void __glXDispSwap_Hint(GLbyte *);
extern void __glXDispSwap_Lightf(GLbyte *);
extern void __glXDispSwap_Lightfv(GLbyte *);
extern void __glXDispSwap_Lighti(GLbyte *);
extern void __glXDispSwap_Lightiv(GLbyte *);
extern void __glXDispSwap_LightModelf(GLbyte *);
extern void __glXDispSwap_LightModelfv(GLbyte *);
extern void __glXDispSwap_LightModeli(GLbyte *);
extern void __glXDispSwap_LightModeliv(GLbyte *);
extern void __glXDispSwap_LineStipple(GLbyte *);
extern void __glXDispSwap_LineWidth(GLbyte *);
extern void __glXDispSwap_Materialf(GLbyte *);
extern void __glXDispSwap_Materialfv(GLbyte *);
extern void __glXDispSwap_Materiali(GLbyte *);
extern void __glXDispSwap_Materialiv(GLbyte *);
extern void __glXDispSwap_PointSize(GLbyte *);
extern void __glXDispSwap_PolygonMode(GLbyte *);
extern void __glXDispSwap_PolygonStipple(GLbyte *);
extern void __glXDispSwap_Scissor(GLbyte *);
extern void __glXDispSwap_ShadeModel(GLbyte *);
extern void __glXDispSwap_TexParameterf(GLbyte *);
extern void __glXDispSwap_TexParameterfv(GLbyte *);
extern void __glXDispSwap_TexParameteri(GLbyte *);
extern void __glXDispSwap_TexParameteriv(GLbyte *);
extern void __glXDispSwap_TexImage1D(GLbyte *);
extern void __glXDispSwap_TexImage2D(GLbyte *);
extern void __glXDispSwap_TexEnvf(GLbyte *);
extern void __glXDispSwap_TexEnvfv(GLbyte *);
extern void __glXDispSwap_TexEnvi(GLbyte *);
extern void __glXDispSwap_TexEnviv(GLbyte *);
extern void __glXDispSwap_TexGend(GLbyte *);
extern void __glXDispSwap_TexGendv(GLbyte *);
extern void __glXDispSwap_TexGenf(GLbyte *);
extern void __glXDispSwap_TexGenfv(GLbyte *);
extern void __glXDispSwap_TexGeni(GLbyte *);
extern void __glXDispSwap_TexGeniv(GLbyte *);
extern void __glXDispSwap_InitNames(GLbyte *);
extern void __glXDispSwap_LoadName(GLbyte *);
extern void __glXDispSwap_PassThrough(GLbyte *);
extern void __glXDispSwap_PopName(GLbyte *);
extern void __glXDispSwap_PushName(GLbyte *);
extern void __glXDispSwap_DrawBuffer(GLbyte *);
extern void __glXDispSwap_Clear(GLbyte *);
extern void __glXDispSwap_ClearAccum(GLbyte *);
extern void __glXDispSwap_ClearIndex(GLbyte *);
extern void __glXDispSwap_ClearColor(GLbyte *);
extern void __glXDispSwap_ClearStencil(GLbyte *);
extern void __glXDispSwap_ClearDepth(GLbyte *);
extern void __glXDispSwap_StencilMask(GLbyte *);
extern void __glXDispSwap_ColorMask(GLbyte *);
extern void __glXDispSwap_DepthMask(GLbyte *);
extern void __glXDispSwap_IndexMask(GLbyte *);
extern void __glXDispSwap_Accum(GLbyte *);
extern void __glXDispSwap_Disable(GLbyte *);
extern void __glXDispSwap_Enable(GLbyte *);
extern void __glXDispSwap_PopAttrib(GLbyte *);
extern void __glXDispSwap_PushAttrib(GLbyte *);
extern void __glXDispSwap_Map1d(GLbyte *);
extern void __glXDispSwap_Map1f(GLbyte *);
extern void __glXDispSwap_Map2d(GLbyte *);
extern void __glXDispSwap_Map2f(GLbyte *);
extern void __glXDispSwap_MapGrid1d(GLbyte *);
extern void __glXDispSwap_MapGrid1f(GLbyte *);
extern void __glXDispSwap_MapGrid2d(GLbyte *);
extern void __glXDispSwap_MapGrid2f(GLbyte *);
extern void __glXDispSwap_EvalCoord1dv(GLbyte *);
extern void __glXDispSwap_EvalCoord1fv(GLbyte *);
extern void __glXDispSwap_EvalCoord2dv(GLbyte *);
extern void __glXDispSwap_EvalCoord2fv(GLbyte *);
extern void __glXDispSwap_EvalMesh1(GLbyte *);
extern void __glXDispSwap_EvalPoint1(GLbyte *);
extern void __glXDispSwap_EvalMesh2(GLbyte *);
extern void __glXDispSwap_EvalPoint2(GLbyte *);
extern void __glXDispSwap_AlphaFunc(GLbyte *);
extern void __glXDispSwap_BlendFunc(GLbyte *);
extern void __glXDispSwap_LogicOp(GLbyte *);
extern void __glXDispSwap_StencilFunc(GLbyte *);
extern void __glXDispSwap_StencilOp(GLbyte *);
extern void __glXDispSwap_DepthFunc(GLbyte *);
extern void __glXDispSwap_PixelZoom(GLbyte *);
extern void __glXDispSwap_PixelTransferf(GLbyte *);
extern void __glXDispSwap_PixelTransferi(GLbyte *);
extern void __glXDispSwap_PixelMapfv(GLbyte *);
extern void __glXDispSwap_PixelMapuiv(GLbyte *);
extern void __glXDispSwap_PixelMapusv(GLbyte *);
extern void __glXDispSwap_ReadBuffer(GLbyte *);
extern void __glXDispSwap_CopyPixels(GLbyte *);
extern void __glXDispSwap_DrawPixels(GLbyte *);
extern void __glXDispSwap_DepthRange(GLbyte *);
extern void __glXDispSwap_Frustum(GLbyte *);
extern void __glXDispSwap_LoadIdentity(GLbyte *);
extern void __glXDispSwap_LoadMatrixf(GLbyte *);
extern void __glXDispSwap_LoadMatrixd(GLbyte *);
extern void __glXDispSwap_MatrixMode(GLbyte *);
extern void __glXDispSwap_MultMatrixf(GLbyte *);
extern void __glXDispSwap_MultMatrixd(GLbyte *);
extern void __glXDispSwap_Ortho(GLbyte *);
extern void __glXDispSwap_PopMatrix(GLbyte *);
extern void __glXDispSwap_PushMatrix(GLbyte *);
extern void __glXDispSwap_Rotated(GLbyte *);
extern void __glXDispSwap_Rotatef(GLbyte *);
extern void __glXDispSwap_Scaled(GLbyte *);
extern void __glXDispSwap_Scalef(GLbyte *);
extern void __glXDispSwap_Translated(GLbyte *);
extern void __glXDispSwap_Translatef(GLbyte *);
extern void __glXDispSwap_Viewport(GLbyte *);
extern void __glXDispSwap_PolygonOffset(GLbyte *);
extern void __glXDispSwap_DrawArrays(GLbyte *);
extern void __glXDispSwap_Indexubv(GLbyte *);
extern void __glXDispSwap_ColorSubTable(GLbyte *);
extern void __glXDispSwap_CopyColorSubTable(GLbyte *);
extern void __glXDispSwap_ActiveTextureARB(GLbyte *);
extern void __glXDispSwap_MultiTexCoord1dvARB(GLbyte *);
extern void __glXDispSwap_MultiTexCoord1fvARB(GLbyte *);
extern void __glXDispSwap_MultiTexCoord1ivARB(GLbyte *);
extern void __glXDispSwap_MultiTexCoord1svARB(GLbyte *);
extern void __glXDispSwap_MultiTexCoord2dvARB(GLbyte *);
extern void __glXDispSwap_MultiTexCoord2fvARB(GLbyte *);
extern void __glXDispSwap_MultiTexCoord2ivARB(GLbyte *);
extern void __glXDispSwap_MultiTexCoord2svARB(GLbyte *);
extern void __glXDispSwap_MultiTexCoord3dvARB(GLbyte *);
extern void __glXDispSwap_MultiTexCoord3fvARB(GLbyte *);
extern void __glXDispSwap_MultiTexCoord3ivARB(GLbyte *);
extern void __glXDispSwap_MultiTexCoord3svARB(GLbyte *);
extern void __glXDispSwap_MultiTexCoord4dvARB(GLbyte *);
extern void __glXDispSwap_MultiTexCoord4fvARB(GLbyte *);
extern void __glXDispSwap_MultiTexCoord4ivARB(GLbyte *);
extern void __glXDispSwap_MultiTexCoord4svARB(GLbyte *);

extern void __glXDispSwap_TexSubImage1D(GLbyte *);
extern void __glXDispSwap_TexSubImage2D(GLbyte *);
extern void __glXDispSwap_ConvolutionFilter1D(GLbyte *);
extern void __glXDispSwap_ConvolutionFilter2D(GLbyte *);
extern void __glXDispSwap_ConvolutionParameterfv(GLbyte *);
extern void __glXDispSwap_ConvolutionParameteriv(GLbyte *);
extern void __glXDispSwap_CopyConvolutionFilter1D(GLbyte *);
extern void __glXDispSwap_CopyConvolutionFilter2D(GLbyte *);
extern void __glXDispSwap_SeparableFilter2D(GLbyte *);
extern void __glXDispSwap_TexImage3D(GLbyte *);
extern void __glXDispSwap_TexSubImage3D(GLbyte *);
extern void __glXDispSwap_DrawArrays(GLbyte *);
extern void __glXDispSwap_PrioritizeTextures(GLbyte *);
extern void __glXDispSwap_CopyTexImage1D(GLbyte *);
extern void __glXDispSwap_CopyTexImage2D(GLbyte *);
extern void __glXDispSwap_CopyTexSubImage1D(GLbyte *);
extern void __glXDispSwap_CopyTexSubImage2D(GLbyte *);
extern void __glXDispSwap_CopyTexSubImage3D(GLbyte *);

#define __GLX_MIN_GLXCMD_OPCODE 1
#define __GLX_MAX_GLXCMD_OPCODE 20
#define __GLX_MIN_RENDER_OPCODE 1
#define __GLX_MAX_RENDER_OPCODE 213
#define __GLX_MIN_SINGLE_OPCODE 1
#define __GLX_MAX_SINGLE_OPCODE 159
#define __GLX_SINGLE_TABLE_SIZE 160
#define __GLX_RENDER_TABLE_SIZE 214

#define __GLX_MIN_RENDER_OPCODE_EXT 4096
#define __GLX_MAX_RENDER_OPCODE_EXT 4123

extern __GLXdispatchSingleProcPtr __glXSingleTable[__GLX_SINGLE_TABLE_SIZE];
extern __GLXdispatchSingleProcPtr __glXSwapSingleTable[__GLX_SINGLE_TABLE_SIZE];
#endif                          /* _GLX_g_disptab_h_ */
