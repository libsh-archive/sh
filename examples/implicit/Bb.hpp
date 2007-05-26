#ifndef BB_HPP
#define BB_HPP

// Hit space while the program is running to get a help message 

// USE_STACK means use a tuple for a stack and do typical branch and bound
// (subdivide by two each time)
//
// USE_STACK false means don't keep a stack.  
// Just keep start, mid, end and move mid closer to start by half
// if there is potentially a hit in [start,mid], else continue on [mid,end]
#define USE_STACK false 
#define DUMP_SYMS false 

void dump(Program &foo, string desc) {
  foo.node()->dump(desc);
}

void printAll(const string& message, Program& foo) {
  cout << message << endl;
  cout << "Interface:" << endl;
  cout << foo.describe_interface() << endl;
  cout << "Binding spec:" << endl;
  cout << foo.describe_binding_spec() << endl;
  assert(foo.binding_spec.size() == foo.stream_inputs.size() + foo.uniform_inputs.size());
}

// given an interval program that computes some (reasonably nice) function g: Attrib1i_f -> Attrib1i_f,
// this computes a root and sets hasroot to true if there is a root in the part of the domain  
Program trace(const Program& func) {
  Program result = SH_BEGIN_PROGRAM() {
    InputPoint3f SH_DECL(origin);
    InputVector3f SH_DECL(dir);
    InputAttrib1f SH_DECL(t);
    OutputAttrib1f SH_DECL(value);
    Attrib3f SH_DECL(p) = mad(t, dir, origin);
    value = func(p);
  } SH_END;
  result.node()->collectAll();
  result.name("tracer_" + func.name());
  return result;
}

/* Returns where a the line alpha * x + beta line intersects zero 
 * (may be numerically unstable when alpha is small or large) */
Attrib1f isct_zero(const Attrib1f& alpha, const Attrib1f& beta) {
  return -beta / alpha;
}



/* Branch and Bound, with stack or sans stack.
 *
 * Commonly branch and bound approaches use either BFS or DFS (or some hybrid)
 * of the search space.  
 * 
 * Tried no-stack alternatives
 *  * always split remaining region in half
 *    - lots of repeated work due to wrapping effect on large domains
 *  * always move either up or down in split size
 *    - may be better 
 *
 * Convergence compared to regular DFS style in optimal case is the same
 * for a hit (@todo prove), O(1) for stack vs. O(log(n)) for miss.
 * Otherwise, it depends on convergence rate of the type of range arithmetic.
 * (TODO proof that with "well behaved" functions, convergence is in the 
 * same order as the usual stack based alg with a small constant)
 * (TODO define "well behaved")
 */ 
Program firsthit(Program tracer, bool use_aa, DebugMode debugMode, TracerFrame& tframe, const InputTexCoord2f& texcoord)
{

  Program aa_tracer; 
  Program ia_tracer; 
  if(use_aa || debugMode == TRACE) {
    aa_tracer = affine_inclusion_syms(tracer);
    aa_tracer.name("aa_tracer_" + tracer.name());
    dump(aa_tracer, "aa_tracer");
  } 
  if(!use_aa || debugMode == TRACE) {
    ia_tracer = inclusion(tracer);
    ia_tracer.name("ia_tracer_" + tracer.name());
    dump(ia_tracer, "ia_tracer");
  }
  /*
  if(DUMP_SYMS) {
    AaVarSymsMap empty;
    ostringstream sout;
    switch(rangeMode) {
        case IA: sout << "IA_"; break;
        case AA: sout << "AA_"; break;
        case AA_NOUC: sout << "AA-nouc_"; break;
    }
    sout << DebugModeName[debugMode] << "_";
    sout << "symfile.csv";
    i_tracer.meta("aa_symfile", sout.str());
    cout << "dumping symfile " << sout.str() << endl;
    placeAaSyms(i_tracer.node(), empty, true);
    cout << "done dumping symfile" << endl;
  }
  */

  Context::current()->disable_optimization("propagation");
  Program result = SH_BEGIN_PROGRAM() {
    InputAttrib1i_f SH_DECL(range); // range to search

    OutputAttrib1f SH_DECL(hashit); // bool to indicate if there was a hit
    OutputAttrib1f SH_DECL(hit); // hit location

    Attrib1f SH_DECL(start) = range_lo(range);
    Attrib1f SH_DECL(end) = range_hi(range);
    Attrib1f SH_DECL(mid) = lerp(0.5f, start, end); 
    Attrib1f SH_DECL(rangeWidth) = range_width(range);
    Attrib1i_f SH_DECL(resultRangeIA);
    Attrib1i_f SH_DECL(resultRangeAA);
    Attrib3f SH_DECL(extraDebug) = ConstColor3f(0.75f, 0.75f, 0.75f);

    Attrib1f SH_DECL(count) = 0.0f;

    Attrib1f SH_DECL(cutoff) = debugMode == PLOT ? tframe.loop_highlight : tframe.loop_cutoff; 
    Attrib1f SH_DECL(hitcond) = 0.0f;

#if USE_STACK
    static const int MAX_STACK = 32;
    ScalarStack<MAX_STACK, float> stack;
    stack.push(start, end);

    SH_WHILE(stack.count() >= 2 && !(hitcond && (mid - start < tframe.eps))) { 
      start = stack[0]; 
      mid = stack[1];
      if(use_aa) {
        stack.pop(2);
      } else {
        stack.pop(1);
      }
#else

    // Check for hits until feasible range drops below tframe.eps
    //SH_WHILE(mid - start > tframe.eps && end - mid > tframe.eps) { 
    SH_WHILE(mid - start > tframe.eps && end - mid > tframe.eps) { 
#endif
      count += 1.0f;

      if(use_aa || debugMode == TRACE) {
        Attrib1a_f SH_DECL(traceRange) = make_interval(start - tframe.traceEps, mid + tframe.traceEps);
        Attrib1a_f SH_DECL(traceResult);
        traceResult = aa_tracer(traceRange);
        resultRangeAA = traceResult;
        if(use_aa) {
          Attrib1f SH_DECL(resultCenter) = range_center(traceResult);
          Attrib1f SH_DECL(resultRadius) = range_radius(traceResult);
          Attrib1f SH_DECL(inError) = affine_lasterr(traceResult, traceRange);
          Attrib1f SH_DECL(otherError) = abs(resultRadius - abs(inError));
          Attrib1f SH_DECL(resultLo) = resultCenter - resultRadius;
          Attrib1f SH_DECL(resultHi) = resultCenter + resultRadius;

          // If there's a potential hit, subdivide [start,mid], 
          // else discard and continue with [mid,end]
          // Attrib1f hitcond = range_lo(range_contains(traceResult, 0));
          //hitcond = range_lo(traceResult) < tframe.traceEps && -tframe.traceEps < range_hi(traceResult);

          /* Find the range in the parallelogram that intersects zero */ 
          Attrib1f slopeZero = abs(inError) < tframe.traceEps; 
          Attrib1f nextStart, nextEnd;
          hitcond = 0.0f;
          SH_IF(slopeZero || otherError > 1e2) {
            hitcond = resultLo < tframe.traceEps && -tframe.traceEps < resultHi;
            nextStart = start;
            nextEnd = mid;
          } SH_ELSE { /* use parallelograms */
            Attrib1f isct1 =  isct_zero(inError, resultCenter + otherError);  
            Attrib1f isct2 =  isct_zero(inError, resultCenter - otherError);  

            /* Represents start/end range in terms of the input partial deviation value in [-1, 1] */ 
            Attrib1f startp, startn, midp, midn; 

            /* positive slope (inError) case */
            startp = max(isct1, -1.0f); 
            midp = min(isct2, 1.0f); 

            /* negative slope case */
            startn = max(isct2, -1.0f); 
            midn = min(isct1, 1.0f); 

            Attrib1f slopePositive = inError > 0;
            Attrib1f zeroStart = lerp(slopePositive, startp, startn);
            Attrib1f zeroEnd = lerp(slopePositive, midp, midn);
            hitcond = zeroStart < zeroEnd; 

            ConstAttrib1f HALF(0.5f);
            //extraDebug(0) = zeroStart > 0;
            //extraDebug(1) = zeroEnd > 0;
            //extraDebug(2) = hitcond;
            nextStart = lerp(mad(zeroStart, HALF, HALF), mid, start); 
            nextEnd = lerp(mad(zeroEnd, HALF, HALF), mid, start); 
          } SH_ENDIF;

#if USE_STACK
          SH_IF(hitcond) {
            Attrib1f nextMid = lerp(0.5f, nextStart, nextEnd);
            stack.push(nextStart, nextMid, nextMid, nextEnd); 
          } SH_ENDIF; 
#else
          SH_IF(hitcond) {
            start = nextStart;
            mid = lerp(0.5f, nextStart, nextEnd);
          } SH_ELSE {
            /* jump up in one size, unless this puts mid past end */
            Attrib1f nextStep = (mid - start) * 2.0;
            nextStep = lerp(nextStep < tframe.eps, tframe.eps, nextStep);
            Attrib1f nextMid = mid + nextStep; 
            nextMid = lerp(nextMid > end, end, nextMid);
            start = mid;
            mid = nextMid;
          } SH_ENDIF;
          //start = lerp(hitcond, start, mid);
          //mid = lerp(0.5f, mid, lerp(hitcond, start, end)); 
#endif
        }
      } 
      if(!use_aa || debugMode == TRACE) {
        Attrib1i_f SH_DECL(traceRange) = make_interval(start - tframe.traceEps, mid + tframe.traceEps);
        Attrib1i_f SH_DECL(traceResult);
        traceResult = ia_tracer(traceRange);
        resultRangeIA = traceResult;

        if(!use_aa) {

          // If there's a potential hit, subdivide [start,mid], 
          // else discard and continue with [mid,end]
          // Attrib1f hitcond = range_lo(range_contains(traceResult, 0));
          hitcond = range_lo(traceResult) < tframe.traceEps && -tframe.traceEps < range_hi(traceResult);
#if USE_STACK
          SH_IF(hitcond) {
            Attrib1f nextMid = lerp(0.5f, start, mid);
            stack.push(start, nextMid); 
          } SH_ENDIF; 
#else
          SH_IF(hitcond) {
            mid = lerp(0.5f, start, mid);
          } SH_ELSE {
            Attrib1f nextStep = (mid - start) * 2.0;
            nextStep = lerp(nextStep < tframe.eps, tframe.eps, nextStep);
            Attrib1f nextMid = mid + nextStep; 
            nextMid = lerp(nextMid > end, end, nextMid);
            start = mid;
            mid = nextMid;
          } SH_ENDIF;
          //start = lerp(hitcond, start, mid);
          //mid = lerp(0.5f, mid, lerp(hitcond, start, end)); 
#endif
        }
      }
      if(debugMode == CUTOFF || debugMode == PLOT) {

        // do something to end the loop
#if USE_STACK
        SH_IF(count > cutoff) {
          stack.clear();
        } SH_ENDIF;
#else
        end = lerp(count > cutoff, mid - 1.0, end);
#endif
      } else if (debugMode == TRACE) {
        Attrib1f u = texcoord(0);
#if USE_STACK
        Attrib1f v = texcoord(1) * 3;
#else
        Attrib1f v = texcoord(1) * 2;
#endif
        Attrib1f done = count > u * tframe.loop_cutoff; 
        Attrib1f inHighlight = count > tframe.loop_highlight && 
                               count <= (tframe.loop_highlight + 1); 

        ConstAttrib1f gapSize = 0.05;
        Attrib1f graphHeight = 1.0 - gapSize * 0.5;
        Attrib1f graphStart2 = graphHeight + gapSize;
        Attrib1f graphEnd2 = graphStart2 + graphHeight; 
        Attrib1f graphStart3 = graphEnd2 + gapSize; 
        Attrib1f inDomainGraph = v < graphHeight; 
        Attrib1f inGap1 = v < graphStart2;
        Attrib1f inRangeGraph = v < graphEnd2; 
        Attrib1f inGap2 = v < graphStart3;

        /* domain graph on bottom */
        Attrib1f rl = range_lo(range);
        Attrib1f s = (start - rl) / rangeWidth * graphHeight; 
        Attrib1f m = (mid - rl) / rangeWidth * graphHeight; 
        Attrib1f insm = v > s && v < m; 
        Attrib1f inme = v > m; 

        /* range graph in middle */
        Attrib1f dv = abs(dy(v));
        Attrib1f vv = ((v - graphStart2) / graphHeight - 0.5) * tframe.graphScale;
        Attrib1f inRangeAA = vv + dv > range_lo(resultRangeAA) && vv - dv < range_hi(resultRangeAA); 
        Attrib1f inRangeIA = vv + dv > range_lo(resultRangeIA) && vv - dv < range_hi(resultRangeIA); 
        Attrib1f inRangeBoth = inRangeAA && inRangeIA; 
        Attrib1f inZero = abs(vv) < tframe.graphScale / 200.0;

        /* stack count graph on top */
#if USE_STACK
        vv = ((v - graphStart3) / graphHeight);
        Attrib1f inStack = (vv * MAX_STACK < stack.count()) && vv > 0; 
#endif

        /* Pick a final color */
        ConstColor3f smColor(0.9, 0.6, 0.5); 
        ConstColor3f meColor(0.5, 0.6, 0.8); 
        ConstColor3f bothColor(0.9, 0.5, 0.7); 
        ConstColor3f noColor(0.75, 0.75, 0.75);
        ConstColor3f fullColor(1, 0, 0);
        ConstColor3f TufteOrange(1.0f, 0.62f, .035f);

        //Color3f bkgndColor = lerp(inHighlight, TufteOrange, noColor);
        Color3f bkgndColor = lerp(inHighlight, TufteOrange, extraDebug); 
        Color3f domainColor = lerp(insm, smColor, lerp(inme, meColor, bkgndColor)); 
        Color3f rangeColor = lerp(inRangeBoth, bothColor, 
                             lerp(inRangeAA, smColor,
                             lerp(inRangeIA, meColor, bkgndColor)));
        rangeColor = lerp(inZero, rangeColor * 0.75, rangeColor);
#if USE_STACK
        Color3f stackColor = lerp(inStack, bothColor, bkgndColor);
#endif

        Color3f dbgColor = lerp(inDomainGraph, domainColor, 
                           lerp(inGap1, bkgndColor,
#if USE_STACK
                           lerp(inRangeGraph, rangeColor,
                           lerp(inGap2, bkgndColor, stackColor))));
#else                       
                           rangeColor));
#endif
        //dbgColor = lerp(stack.full(), dbgColor * fullColor, dbgColor);  
        shDBG(dbgColor);
#if USE_STACK
        SH_IF(done) {
          stack.clear();
        } SH_ENDIF;
#else
        end = lerp(done, mid - 1.0, end);
#endif
      }
#if USE_STACK
    } SH_ENDWHILE;
#else
    //} SH_ENDWHILE;
    } SH_ENDWHILE;
#endif

    switch(debugMode) {
      case LOOP:
        hashit = count < cutoff;
        hit = count * (1.0f / cutoff);
        break;
      case CUTOFF:
        hashit = range_hi(range) - mid > tframe.eps; 
        hit = (mid - range_lo(range)) / rangeWidth; 
        break;
      case PLOT:
        hashit = start; 
        hit = mid; 
        break;
      default:
        hashit = hitcond; 
        hit = lerp(0.5f, start, mid);
        break;
    }
    if(debugMode == LOOP) {
    } else {
    }
  } SH_END;
  Context::current()->enable_optimization("propagation");
  dump(result, "firsthit");
  std::cout << "Optimizing firsthit" << endl;
  optimize(result);
  dump(result, "firsthit-prop");
  return result;
}
#endif
