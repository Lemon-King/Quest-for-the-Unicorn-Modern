/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The canvas scaled as the GDI build's CPU viewport did it. Window pixel i covers canvas span i*size/scaled ..
   (i+1)*size/scaled on each axis: at most two canvas pixels, blended by how much of the window pixel lies over the
   second, in 256ths. Whole-number scales stay sharp; integer arithmetic keeps every channel identical to the CPU. */

Texture2D<float4> canvas : register(t0);

cbuffer Placement : register(b0) {
  uint2 size;
  uint2 scaled;
  int2  origin;
  int2  padding;
  };

struct Sample {
  uint source;
  uint blend;
  };

/* Unsigned, which divides faster and gives the CPU's results: the viewport only rasterizes window pixels inside the
   placement, so nothing here is negative except an overlap that means no blend */
Sample sampleAxis(uint i, uint axisSize, uint axisScaled) {
  Sample result;
  int    overlap;
  result.source = i * axisSize / axisScaled;
  overlap = (int)((i + 1) * axisSize) - (int)((result.source + 1) * axisScaled);
  result.blend = (overlap > 0) ? (uint)overlap * 256 / axisSize : 0;
  return result;
  }

/* B8G8R8A8_UNORM reads back as R, G, B; round() undoes the conversion to float exactly */
uint3 texel(uint x, uint y) {
  return (uint3)round(canvas.Load(int3(x, y, 0)).rgb * 255.0);
  }

/* weight/256 of second into first, per channel, as _rcs_blendPixel */
uint3 mix(uint3 first, uint3 second, uint weight) {
  return (first * (256 - weight) + second * weight) >> 8;
  }

uint3 sampleRow(uint y, Sample column) {
  uint3 left = texel(column.source, y);
  if (column.blend == 0) return left;
  return mix(left, texel(column.source + 1, y), column.blend);
  }

float4 main(float4 position : SV_Position) : SV_Target {
  uint2  pixel = (uint2)(int2(position.xy) - origin);
  Sample column = sampleAxis(pixel.x, size.x, scaled.x);
  Sample row = sampleAxis(pixel.y, size.y, scaled.y);
  uint3  color = sampleRow(row.source, column);
  if (row.blend != 0) color = mix(color, sampleRow(row.source + 1, column), row.blend);
  return float4(color / 255.0, 1.0);
  }
