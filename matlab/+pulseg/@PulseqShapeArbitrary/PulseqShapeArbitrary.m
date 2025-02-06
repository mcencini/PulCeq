classdef PulseqShapeArbitrary
    %PULSEQSHAPEARBITRARY Represents an arbitrary waveform shape.

    properties
        samples single
    end

    methods
        function obj = PulseqShapeArbitrary(samples)
            if nargin > 0
                obj.samples = samples;
            end
        end

        function bytes = to_bytes(obj, byteswap)
            % Serialize PulseqShapeArbitrary to bytes.
            %
            % Parameters
            % ----------
            % byteswap : logical, optional
            %     Whether to byteswap (True -> big-endian, False -> little-endian), default is False
            %
            % Returns
            % -------
            % bytes : uint8 array
            %     Serialized byte representation of the object.
  
            if nargin < 2
                byteswap = false;
            end
            
            % Enforce precision
            obj_nSamples = int32(length(obj.samples));
            obj_samples = single(obj.samples);
            
            % Perform byteswap if requested
            if byteswap
                obj_nSamples = swapbytes(obj_nSamples);
                obj_samples = swapbytes(obj_samples);
            end

            % Convert to bytes
            b_nSamples = typecast(obj_nSamples, 'uint8');
            b_samples = typecast(obj_samples, 'uint8');
                        
            % Concatenate
            bytes = [b_nSamples, b_samples];
        end
    end

    methods (Static)
        function obj = from_bytes(data, byteswap)
            % Deserialize PulseqShapeArbitrary from bytes.
            %
            % Parameters
            % ----------
            % data : uint8 array
            %     Byte data to deserialize.
            % byteswap : logical, optional
            %     Whether to byteswap (True -> big-endian, False -> little-endian), default is False
            %
            % Returns
            % -------
            % obj : PulseqShapeArbitrary
            %     Deserialized object.

            if nargin < 2
                byteswap = false;
            end
            
            % Extract number of samples
            b_nSamples = data(1:4);
                        
            % Convert to int
            nSamples = typecast(b_nSamples, 'int32');
            
            % Perform byteswap if requested
            if byteswap
                nSamples = swapbytes(nSamples);
            end
            
            % Extract samples
            b_samples = typecast(data(5:5+(nSamples * 4)-1), 'single');
                        
            % Convert to float
            samples = typecast(b_samples, 'single');
            
            % Perform byteswap if requested
            if byteswap
                samples = swapbytes(samples);
            end
            
            % Return object
            obj = pulseg.PulseqShapeArbitrary(samples);
        end
    end
end
