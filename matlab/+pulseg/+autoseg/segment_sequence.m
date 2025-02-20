function [sections_def, sections_lut] = segment_sequence(arr, min_length)
    % SEGMENT_SEQUENCE Identifies repeating subsequences in a sequence.
    %
    % This function detects patterns in `arr` and merges forward and 
    % reverse segmentations for consistency. If only one segment is found, 
    % it applies additional heuristics to identify periodicity.
    %
    % Parameters
    % ----------
    % arr : numeric array
    %     Input sequence to analyze.
    % min_length : int, optional
    %     Minimum candidate length to consider (default is 1).
    %
    % Returns
    % -------
    % sections_def : cell array
    %     List of unique segment definitions.
    % sections_lut : numeric array
    %     Lookup table mapping elements of `arr` to segment IDs.

    % Set default min_length if not provided
    if nargin < 2
        min_length = 1;
    end

    % Identify segments using forward and reverse pattern detection
    subseq_fwd = pulseg.autoseg.find_patterns(arr, false, min_length);
    subseq_rev = pulseg.autoseg.find_patterns(arr, true, min_length);

    % Merge segmentations for consistency
    merged_segments = sub_merge_segmentations(subseq_fwd, subseq_rev);
    nsegments = length(merged_segments);

    % Initialize output variables
    sections_def = {};
    sections_lut = [];

    % Handle case where only one segment is detected
    if nsegments == 1
        [sections_def_tmp, sections_lut] = sub_find_sections_def(merged_segments{1});
        
        % Clean up
        sections_def = {};
        sections_def_tmp = sections_def_tmp{1};
        nrows = size(sections_def_tmp, 1);
        for n = 1:nrows
            sections_def{n} = sections_def_tmp(n, :);
        end
            
        return;  % No need to continue further
    end

    % Process multiple segments and combine results
    offset = 0;
    for n = 1:nsegments
        [current_def, current_lut] = sub_find_sections_def(merged_segments{n});
        
        % Append new segment definitions
        sections_def = cat(2, sections_def, current_def);
        
        % Adjust and append lookup table
        sections_lut = cat(1, sections_lut, current_lut(:) + offset);
        offset = max(sections_lut);  % Update offset for next iteration
    end
end

function merged = sub_merge_segmentations(seg1, seg2)
    % Ensure consistent segmentation by merging two segmentations.
    
    if sub_is_equal(seg1, seg2)
        merged = cellfun(@(s) repmat(s{1}, 1, s{2}), seg1, 'UniformOutput', false);
    else

        merged = {};
        i = 1;
        j = 1;

        while i <= length(seg1) && j <= length(seg2)
            s1 = seg1{i}{1}; r1 = seg1{i}{2};
            s2 = seg2{j}{1}; r2 = seg2{j}{2};

            expanded_s1 = repmat(s1, 1, r1);
            expanded_s2 = repmat(s2, 1, r2);

            if isequal(expanded_s1, expanded_s2)
                merged{end+1} = expanded_s1;
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
                            combined_s1 = [combined_s1, expanded_s1];
                        end
                    else
                        j = j + 1;
                        if j <= length(seg2)
                            s2 = seg2{j}{1}; r2 = seg2{j}{2};
                            expanded_s2 = repmat(s2, 1, r2);
                            combined_s2 = [combined_s2, expanded_s2];
                        end
                    end
                end

                merged{end+1} = combined_s1;
                i = i + 1;
                j = j + 1;
            end
        end
    end
end

function [sections_def, sections_lut] = sub_find_sections_def(arr)
    % Identify repeating sections that start with the first element in the array.
    % This function detects periodic patterns and assigns unique IDs to them.

    % Ensure arr is a column vector
    arr = arr(:);
    first_id = arr(1);
    
    % Find all occurrences of first_id
    indices = find(arr == first_id);
    num_occurrences = length(indices);

    % If there are not enough occurrences, return the entire array as one section
    if num_occurrences < 2
        sections_def = {arr(:).'};  
        sections_lut = ones(size(arr));  
        return;
    end

    % Compute section lengths based on distances between consecutive occurrences of first_id
    section_lengths = [diff(indices); length(arr)-indices(end)+1];
    
    % Identify unique section lengths
    [unique_lengths, ~, section_IDs] = unique(section_lengths, 'stable');

    % Initialize output variables
    sections_def = {};
    sections_lut = zeros(size(arr));

    % Process each unique section length
    offset = 0;
    for i = 1:length(unique_lengths)
        % Get all section start indices for the current length
        section_start_idx = indices(section_IDs == i);
        section_len = unique_lengths(i);

        % Extract corresponding sections
        all_section_idx = section_start_idx + (0:section_len-1); % Compute full indices
        all_section_idx = all_section_idx';  % Transpose for correct indexing
        all_section_idx = all_section_idx(:); % Flatten

        % Retrieve section data
        section_data = arr(all_section_idx);
        
        % Reshape to group into separate sections
        section_data = reshape(section_data, [section_len, numel(section_data) / section_len]);

        % Identify unique section patterns
        [unique_sections, ~, unique_section_IDs] = unique(section_data', 'rows', 'stable');

        % Store unique section definitions
        sections_def = cat(2, sections_def, {unique_sections});

        % Assign section IDs in lookup table
        sections_lut(all_section_idx) = repelem(unique_section_IDs + offset, section_len);

        % Update offset for section indexing
        offset = max(unique_section_IDs + offset);
    end
end

function equal = sub_is_equal(seg1, seg2)
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
