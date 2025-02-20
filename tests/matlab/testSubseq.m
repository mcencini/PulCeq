classdef testSubseq < matlab.unittest.TestCase
    % Test subsequence identification.

    methods (Test)
        function test_simple_megre(testCase)
            seq = repmat(megre_warmup(), 1, 32);
            seq = [seq, repmat(megre(), 1, 1024)];

            [subseq_def, subseq_lut] = pulseg.autoseg.segment_sequence(seq);

            % Verify number of segments
            testCase.verifyEqual(length(subseq_def), 2);

            % Verify sequence content
            testCase.verifyEqual(subseq_def{1}, megre_warmup());
            testCase.verifyEqual(subseq_def{2}, megre());
            
            % Verify segments lookup table
            testCase.verifyEqual(subseq_lut, [ones(32 * length(megre_warmup()), 1); 2 * ones(1024 * length(megre()), 1)]);
        end

        function test_megre(testCase)
            seq = [noisecal(), repmat(pical(), 1, 64), repmat(megre_warmup(), 1, 32), repmat(megre(), 1, 1024)];

            [subseq_def, subseq_lut] = pulseg.autoseg.segment_sequence(seq);

            % Verify number of segments
            testCase.verifyEqual(length(subseq_def), 4);

            % Verify sequence content
            testCase.verifyEqual(subseq_def{1}, noisecal());
            testCase.verifyEqual(subseq_def{2}, pical());
            testCase.verifyEqual(subseq_def{3}, megre_warmup());
            testCase.verifyEqual(subseq_def{4}, megre());
            
            % Verify segments lookup table
            testCase.verifyEqual(subseq_lut, ...
               [ones(length(noisecal()), 1); ...
                2 * ones(64 * length(pical()), 1); ...
                3 * ones(32 * length(megre_warmup()), 1); ...
                4 * ones(1024 * length(megre()), 1)]);
        end
        
        function test_simple_mprage(testCase)
            seq = [mprage_warmup(), repmat(mprage(), 1, 32)];

            [subseq_def, subseq_lut] = pulseg.autoseg.segment_sequence(seq);

            % Verify number of segments
            testCase.verifyEqual(length(subseq_def), 2);

            % Verify sequence content
            testCase.verifyEqual(subseq_def{1}, mprage_warmup());
            testCase.verifyEqual(subseq_def{2}, mprage());
            
            % Verify segments lookup table
            testCase.verifyEqual(subseq_lut, [ones(length(mprage_warmup()), 1); 2 * ones(32 * length(mprage()), 1)]);
        end

        function test_mprage(testCase)
            seq = [noisecal(), repmat(pical(), 1, 64), mprage_warmup(), repmat([mprage(), repmat(navscan(), 1, 3)], 1, 32)];

            [subseq_def, subseq_lut] = pulseg.autoseg.segment_sequence(seq);

            % Verify number of segments
            testCase.verifyEqual(length(subseq_def), 4);

            % Verify sequence content
            testCase.verifyEqual(subseq_def{1}, noisecal());
            testCase.verifyEqual(subseq_def{2}, pical());
            testCase.verifyEqual(subseq_def{3}, mprage_warmup());
            testCase.verifyEqual(subseq_def{4}, [mprage(), repmat(navscan(), 1, 3)]);
            
            % Verify segments lookup table
            testCase.verifyEqual(subseq_lut, ...
               [ones(length(noisecal()), 1); ...
                2 * ones(64 * length(pical()), 1); ...
                3 * ones(length(mprage_warmup()), 1); ...
                4 * ones(32 * (length(mprage()) + 3 * length(navscan())), 1)]);
        end
    end
end

% Sequence definitions

function seq = noisecal()
    seq = [7, 12]; % (EXC3, READ3)
end

function seq = pical()
    seq = [7, 10, 11, 10, 6]; % (EXC2, PHS2, READ2, PHS2, SPOIL)
end

function seq = navscan()
    seq = [7, 8]; % (EXC1, READ1)
end

function seq = megre()
    seq = [2, 3, 4, 4, 4, 4, 3, 6]; % (EXC0, PHS0, READ0, READ0, READ0, READ0, PHS0, SPOIL)
end

function seq = megre_warmup()
    seq = [2, 3, 5, 5, 5, 5, 3, 6]; % (EXC0, PHS0, DUMMY_READ0, DUMMY_READ0, DUMMY_READ0, DUMMY_READ0, PHS0, SPOIL)
end

function seq = mprage()
    seq = [1, 6, repmat([2, 3, 4, 3, 6], 1, 32)]; % (INV, SPOIL, 32 * [EXC0, PHS0, READ0, PHS0, SPOIL])
end

function seq = mprage_warmup()
    seq = [1, 6, repmat([2, 3, 5, 3, 6], 1, 32)]; % (INV, SPOIL, 32 * [EXC0, PHS0, DUMMY_READ0, PHS0, SPOIL])
end
