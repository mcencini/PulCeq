function result = find_patterns(arr, reverse, min_length)
    % Recursively segment the input sequence into periodic and non-periodic
    % subsequences using MEX-accelerated inner functions.
    %
    % Parameters
    % ----------
    % arr : numeric array
    %     The input sequence of numerical identifiers.
    % reverse : logical (default: true)
    %     Whether to process the array in reverse order.
    % min_length : int (default: 1)
    %     Minimum candidate length to consider.
    %
    % Returns
    % -------
    % result : cell array
    %     Each cell contains:
    %       - A numeric array representing a subsequence.
    %       - An integer representing the number of times the pattern repeats.
    
    if nargin < 2, reverse = true; end
    if nargin < 3, min_length = 1; end

    if isempty(arr)
        result = {};
        return;
    end

    arr = arr(:)';
    if reverse
        arr = fliplr(arr);
    end

    subsequence_definitions = {};

    function recursive_segment(subarr)
        if length(subarr) < min_length
            subsequence_definitions{end+1} = {subarr, 1};
            return;
        end

        % Call the MEX function to find a periodic pattern
        [start_idx, L] = mex_find_periodic_pattern(subarr, min_length);
        if start_idx == -1
            subsequence_definitions{end+1} = {subarr, 1};
            return;
        end

        if start_idx > 1
            % Save non-periodic prefix
            subsequence_definitions{end+1} = {subarr(1:start_idx-1), 1};
        end

        % Count the repetitions using the MEX function
        rep = mex_count_repetitions(subarr, start_idx, L);
        subsequence_definitions{end+1} = {subarr(start_idx:start_idx+L-1), rep};

        remainder = subarr(start_idx + rep * L : end);
        if ~isempty(remainder)
            recursive_segment(remainder);
        end
    end

    recursive_segment(arr);

    if reverse
        result = flip(subsequence_definitions);
        for i = 1:length(result)
            result{i}{1} = fliplr(result{i}{1});
        end
    else
        result = subsequence_definitions;
    end
end
