function result = find_patterns(arr, reverse, min_length)
    % Recursively segment the input sequence into periodic and non-periodic subsequences.
    % Also count the number of consecutive repetitions for each detected periodic pattern.
    %
    % Parameters
    % ----------
    % arr : numeric array
    %     The input sequence of numerical identifiers.
    % reverse : logical
    %     Search patterns starting from the end.
    % min_length : int, optional
    %     Minimum candidate length to consider (default is 1).
    %
    % Returns
    % -------
    % result : cell array
    %     Each cell contains:
    %     - A numeric array representing a subsequence
    %     - An integer representing the number of times the pattern repeats

    if nargin < 2, reverse = true; end
    if nargin < 3, min_length = 1; end

    if isempty(arr)
        result = {};
        return;
    end

    arr = arr(:)'; % Ensure row vector
    if reverse
        arr = fliplr(arr); % Reverse array
    end

    subsequence_definitions = {};

    function recursive_segment(subarr)
        if length(subarr) < min_length
            subsequence_definitions{end+1} = {subarr, 1}; %#ok<AGROW>
            return;
        end

        [start_idx, L] = find_periodic_pattern(subarr, min_length);
        if start_idx == -1
            subsequence_definitions{end+1} = {subarr, 1}; %#ok<AGROW>
            return;
        end

        if start_idx > 1
            subsequence_definitions{end+1} = {subarr(1:start_idx-1), 1}; %#ok<AGROW>
        end

        rep = count_repetitions(subarr, start_idx, L);
        subsequence_definitions{end+1} = {subarr(start_idx:start_idx+L-1), rep}; %#ok<AGROW>

        remainder = subarr(start_idx + rep * L:end);
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

function equal = array_equal(a, b)
    % Compare two 1D arrays element-wise.
    equal = isequal(a, b);
end

function [start_idx, L] = find_periodic_pattern(arr, min_length)
    % Search for a candidate periodic pattern in arr.
    %
    % Returns
    % -------
    % start_idx : int
    %     The index at which a periodic candidate is found.
    % L : int
    %     The length of the periodic candidate.

    n = length(arr);
    for start_idx = 1:n
        max_possible_period = floor((n - start_idx + 1) / 2);

        for L = min_length:max_possible_period
            candidate = arr(start_idx:start_idx+L-1);
            candidate2 = arr(start_idx+L:start_idx+2*L-1);
            if array_equal(candidate, candidate2)
                return;
            end
        end
    end

    start_idx = -1;
    L = -1;
end

function count = count_repetitions(arr, start_idx, L)
    % Count consecutive repetitions of a candidate period.

    count = 0;
    n = length(arr);
    while start_idx + (count + 1) * L - 1 <= n
        segment = arr(start_idx + count * L : start_idx + (count + 1) * L - 1);
        base = arr(start_idx : start_idx + L - 1);
        if array_equal(segment, base)
            count = count + 1;
        else
            break;
        end
    end
end
