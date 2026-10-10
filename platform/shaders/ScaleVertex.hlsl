/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* One triangle covering the viewport; the pixel shader does the work */
float4 main(uint vertex : SV_VertexID) : SV_Position {
  float2 corner = float2((vertex << 1) & 2, vertex & 2);
  return float4(corner * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.0, 1.0);
  }
