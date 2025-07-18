<!-- Authors: Jannis, Li -->

# Concept for automatic color mixing of overlapping strokes
## Quick summary
- When a stroke is made, a check is run to see if the stroke overlaps with any other strokes. For each overlap the following steps are completed.
- The colors of both involved strokes are gotten and `color_mixer()` is called with them with a mixing ration of 50/50.
- The area of the overlap can be found by calculating the intersection of both strokes.
- This returns a new stroke which is given the calculated mixed color. For this stroke, there also needs to be a way to avoid checking for overlaps, since this would lead to an infinite recursion loop.
- Since the overlap is also just a stroke, its color can also be mixed in on subsequently made strokes.

## Detailed explanation
### Premeditations
When a new stroke is made, the following steps are completed to check whether it would overlap with another stroke and thus their colors would need to be mixed on their intersection. Generally, if no intersection is present, the stroke is simply drawn with its own color. If however there is an intersection, a new stroke is generated for that intersection with a color that is mixed between the stroke that was drawn just before and the stroke that was intersected by it. This intersection itself is a stroke with a color, so that it too will be considered for intersections with subsequently made strokes.

### Checking for overlap
Since strokes are collections of spans, checking for overlap between them is not complicated.

- If the bounding boxes of two strokes do not overlap, the strokes do not overlap.
- If the bounding boxes of two strokes overlap, the bounding box of a possible intersection is at most the intersection of both bounding boxes.
- There is an overlap of both strokes if for any span of Y-coordinate A of stroke 1 there is a span of the same Y-coordinate of stroke 2 for which there exists at least one X-coordinate which is part of both spans 1 and 2.

Calculating if there is an overlap might be just as performant as calculating the intersection of two strokes directly. If this is the case, performance can be saved by simply directly calculating the intersection and only continuing on a non-empty result.

### Mixing the colors
The pointer to the colors of the individual strokes can simply be accessed by their property. Then `color_mixer()` can be called with both of those together with a mix of value `0.5` for an even mixing result. It returns a pointer to the calculated color, which is then set as the color of the intersection stroke.

If checking for overlap is done by calculating the intersection itself, then the mixed color must be calculated beforehand depending on implementation details. If a strokes color can be changed easily after it has been created, the colors can be mixed after a non-empty intersection has been gotten and the result then applied. If the color can only be set at the point of generation of a stroke, the mixed color must be calculated before calculating the intersection, even if no intersection will result, because the color would have to be given as a parameter at the point of generation.

### Intersection of strokes
Calculating the intersection of strokes is an extension of calculating whether there is any overlap at all. All spans in the Y-coordinates that both bounding boxes contain are iterated through and the intersection of their respective coordinates become new span for the intersection stroke, with the minimum value in the intersection becoming the new start coordinate and the maximum value becoming the new end coordinate.

After all spans have been iterated through, the bounding box might still have to be changed to fit snugly around the spans, since not every Y or X coordinate has to have had an intersection simply because the bounding boxes of the original strokes overlapped.

### Continuity
On any subsequently made stroke, the strokes that themselves are overlaps of older strokes must also be considered for intersection, which is why it is sensible design that they are independent strokes after having been generated, because the same process will now seamlessly include them.

## Possible extensions
Here are a few further developments that would give the end user more control:

- The option to turn the automatic behavior off.
- The option to set mixing modes or levels for the automatic behavior.
- The option to set a fixed second color when generating a new stroke that will be applied to any intersections the new stroke makes.
