function merged = segment_sequence(arr, min_length)
    % Identify subsequences in an unrolled loop.
    %
    % Parameters
    % ----------
    % arr : numeric array
    %     The input sequence.
    % min_length : int, optional
    %     Minimum candidate length to consider (default is 1).
    %
    % Returns
    % -------
    % merged : cell array
    %     List of segmented sequences.

    if nargin < 2
        min_length = 1;
    end

    subseq_fwd = pulseg.autoseg.find_patterns(arr, false, min_length);
    subseq_rev = pulseg.autoseg.find_patterns(arr, true, min_length);

    merged = merge_segmentations(subseq_fwd, subseq_rev);
end

function merged = merge_segmentations(seg1, seg2)
    % Ensure consistent segmentation by merging two segmentations.
    
    if is_equal(seg1, seg2)
        merged = cellfun(@(s) repmat(s{1}, 1, s{2}), seg1, 'UniformOutput', false);
        return;
    end

    merged = {};
    i = 1;
    j = 1;

    while i <= length(seg1) && j <= length(seg2)
        s1 = seg1{i}{1}; r1 = seg1{i}{2};
        s2 = seg2{j}{1}; r2 = seg2{j}{2};

        expanded_s1 = repmat(s1, 1, r1);
        expanded_s2 = repmat(s2, 1, r2);

        if isequal(expanded_s1, expanded_s2)
            merged{end+1} = expanded_s1; %#ok<AGROW>
            i = i + 1;
            j = j + 1;
        else
            combined_s1 = expanded_s1;
            combined_s2 = expanded_s2;

            while ~isequal(combined_s1, combined_s2)
                if length(combined_s1) < length(combined_s2)
                    i = i + 1;
                    if i <= length(seg1)
                        s1 = seg1{i}{1}; r1 = seg1{i}{2};
                        expanded_s1 = repmat(s1, 1, r1);
                        combined_s1 = [combined_s1, expanded_s1]; %#ok<AGROW>
                    end
                else
                    j = j + 1;
                    if j <= length(seg2)
                        s2 = seg2{j}{1}; r2 = seg2{j}{2};
                        expanded_s2 = repmat(s2, 1, r2);
                        combined_s2 = [combined_s2, expanded_s2]; %#ok<AGROW>
                    end
                end
            end

            merged{end+1} = combined_s1; %#ok<AGROW>
            i = i + 1;
            j = j + 1;
        end
    end
end

function equal = is_equal(seg1, seg2)
    % Check if two segmentations are identical.

    if length(seg1) ~= length(seg2)
        equal = false;
        return;
    end

    for n = 1:length(seg1)
        if seg1{n}{2} ~= seg2{n}{2} || ~isequal(seg1{n}{1}, seg2{n}{1})
            equal = false;
            return;
        end
    end

    equal = true;
end
