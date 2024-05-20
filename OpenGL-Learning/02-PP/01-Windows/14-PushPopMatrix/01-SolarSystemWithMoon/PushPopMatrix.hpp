#include <stdlib.h>
#include <string.h>

#include "pspgl_internal.h"
#include "pspgl_matrix.h"

void glPushMatrix (void)
{
	struct pspgl_context *c = pspgl_curctx;
	struct pspgl_matrix_stack *curstk = c->current_matrix_stack;
	unsigned flags;

	assert(curstk->flags & MF_VFPU);

	/* Make sure VFPU state is written back to memory */
	__pspgl_matrix_sync(c, curstk);

	if (unlikely(++curstk->depth == curstk->limit))
		goto out_error;

	/* No need to copy matrix values, since it's already in the
	   VFPU.  Do need to copy the flags though. */

	flags = c->current_matrix->flags;
	c->current_matrix = &curstk->stack[curstk->depth];
	c->current_matrix->flags = flags;
	return;

  out_error:
	curstk->depth--;
	GLERROR(GL_STACK_OVERFLOW);
}

void glPopMatrix (void)
{
	struct pspgl_context *c = pspgl_curctx;
	struct pspgl_matrix_stack *curstk = c->current_matrix_stack;

	assert(curstk->flags & MF_VFPU);

	if (unlikely(curstk->depth == 0))
		goto out_error;

	c->current_matrix = &curstk->stack[--curstk->depth];
	if (!(curstk->flags & MF_DISABLED))
		curstk->flags |= MF_DIRTY;

	/* Reload VFPU with new top-of-stack */
	__pspgl_matrix_load(c, curstk);
	return;

  out_error:
	GLERROR(GL_STACK_UNDERFLOW);
}
